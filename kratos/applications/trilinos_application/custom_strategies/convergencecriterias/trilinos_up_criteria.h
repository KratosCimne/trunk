/*
==============================================================================
Kratos
A General Purpose Software for Multi-Physics Finite Element Analysis
Version 1.0 (Released on march 05, 2007).

Copyright 2007
Pooyan Dadvand, Riccardo Rossi
pooyan@cimne.upc.edu
rrossi@cimne.upc.edu
CIMNE (International Center for Numerical Methods in Engineering),
Gran Capita' s/n, 08034 Barcelona, Spain

Permission is hereby granted, free  of charge, to any person obtaining
a  copy  of this  software  and  associated  documentation files  (the
"Software"), to  deal in  the Software without  restriction, including
without limitation  the rights to  use, copy, modify,  merge, publish,
distribute,  sublicense and/or  sell copies  of the  Software,  and to
permit persons to whom the Software  is furnished to do so, subject to
the following condition:

Distribution of this code for  any  commercial purpose  is permissible
ONLY BY DIRECT ARRANGEMENT WITH THE COPYRIGHT OWNER.

The  above  copyright  notice  and  this permission  notice  shall  be
included in all copies or substantial portions of the Software.

THE  SOFTWARE IS  PROVIDED  "AS  IS", WITHOUT  WARRANTY  OF ANY  KIND,
EXPRESS OR  IMPLIED, INCLUDING  BUT NOT LIMITED  TO THE  WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT  SHALL THE AUTHORS OR COPYRIGHT HOLDERS  BE LIABLE FOR ANY
CLAIM, DAMAGES OR  OTHER LIABILITY, WHETHER IN AN  ACTION OF CONTRACT,
TORT  OR OTHERWISE, ARISING  FROM, OUT  OF OR  IN CONNECTION  WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

==============================================================================
*/

/* *********************************************************
*
*   Last Modified by:    $Author: kazem $
*   Date:                $Date: 2009-01-15 18:45:31 $
*   Revision:            $Revision: 1.2 $
*
* ***********************************************************/


#if !defined(KRATOS_TRILINOS_UP_CRITERIA )
#define  KRATOS_TRILINOS_UP_CRITERIA


/* System includes */


/* External includes */


/* Project includes */
#include "includes/model_part.h"
#include "includes/define.h"
#include "solving_strategies/convergencecriterias/convergence_criteria.h"
#include "Epetra_MpiComm.h"
#include "mpi.h"

namespace Kratos
{

/**@name Kratos Globals */
/*@{ */


/*@} */
/**@name Type Definitions */
/*@{ */

/*@} */


/**@name  Enum's */
/*@{ */


/*@} */
/**@name  Functions */
/*@{ */



/*@} */
/**@name Kratos Classes */
/*@{ */

/** Short class definition.
Detail class definition.

\URL[Example of use html]{ extended_documentation/no_ex_of_use.html}

\URL[Example of use pdf]{ extended_documentation/no_ex_of_use.pdf}

\URL[Example of use doc]{ extended_documentation/no_ex_of_use.doc}

\URL[Example of use ps]{ extended_documentation/no_ex_of_use.ps}


\URL[Extended documentation html]{ extended_documentation/no_ext_doc.html}

\URL[Extended documentation pdf]{ extended_documentation/no_ext_doc.pdf}

\URL[Extended documentation doc]{ extended_documentation/no_ext_doc.doc}

\URL[Extended documentation ps]{ extended_documentation/no_ext_doc.ps}


*/
template<class TSparseSpace,
         class TDenseSpace
         >
class TrilinosUPCriteria : public ConvergenceCriteria< TSparseSpace, TDenseSpace >
{
public:
    /**@name Type Definitions */
    /*@{ */
    KRATOS_CLASS_POINTER_DEFINITION( TrilinosUPCriteria );

    typedef ConvergenceCriteria< TSparseSpace, TDenseSpace > BaseType;

    typedef TSparseSpace SparseSpaceType;

    typedef typename BaseType::TDataType TDataType;

    typedef typename BaseType::DofsArrayType DofsArrayType;

    typedef typename BaseType::TSystemMatrixType TSystemMatrixType;

    typedef typename BaseType::TSystemVectorType TSystemVectorType;

    /*@} */
    /**@name Life Cycle
    */
    /*@{ */

    /** Constructor.
    */
    TrilinosUPCriteria(
        TDataType VelRatioTolerance,
        TDataType VelAbsTolerance,
        TDataType PrsRatioTolerance,
        TDataType PrsAbsTolerance,
        Epetra_MpiComm& rComm)
        : ConvergenceCriteria< TSparseSpace, TDenseSpace >(),mrComm(rComm)
    {
        mVelRatioTolerance = VelRatioTolerance;
        mVelAbsTolerance = VelAbsTolerance;

        mPrsRatioTolerance = PrsRatioTolerance;
        mPrsAbsTolerance = PrsAbsTolerance;
        //mActualizeRHSIsNeeded = false;
    }

    /** Destructor.
    */
    virtual ~TrilinosUPCriteria() {}


    /*@} */
    /**@name Operators
    */
    /*@{ */

    //****************Pre Criteria*/
    bool PreCriteria(
        ModelPart& r_model_part,
        DofsArrayType& rDofSet,
        const TSystemMatrixType& A,
        const TSystemVectorType& Dx,
        const TSystemVectorType& b
    )
    {
        if (SparseSpaceType::Size(Dx) != 0) //if we are solving for something
        {
            ModelPart::MeshType::NodesContainerType& rLocalNodes = r_model_part.GetCommunicator().LocalMesh().Nodes();
            for (ModelPart::MeshType::NodeIterator it = rLocalNodes.begin(); it != rLocalNodes.end(); it++)
            {
                it->GetValue(VELOCITY) = it->FastGetSolutionStepValue(VELOCITY);
                it->GetValue(PRESSURE) = it->FastGetSolutionStepValue(PRESSURE) / it->FastGetSolutionStepValue(DENSITY);
            }
            return true;

        }
        else //in this case all the displacements are imposed!
        {
            return true;
        }
    }
    //*****************post criteria
    bool PostCriteria(
        ModelPart& r_model_part,
        DofsArrayType& rDofSet,
        const TSystemMatrixType& A,
        const TSystemVectorType& Dx,
        const TSystemVectorType& b
    )
    {
        if (SparseSpaceType::Size(Dx) != 0) //if we are solving for something
        {
            //TDataType mFinalCorrectionNorm = sqrt(std::inner_product(Dx.begin(),Dx.end(),Dx.begin(),TDataType()));
            //TDataType mFinalCorrectionNorm = sqrt(Dot(Dx,Dx));

            double reference_pr_norm = 0.0;
            double difference_pr_norm = 0.0;
            double reference_vel_norm = 0.0;
            double difference_vel_norm = 0.0;

            ModelPart::MeshType::NodesContainerType& rLocalNodes = r_model_part.GetCommunicator().LocalMesh().Nodes();
            for (ModelPart::MeshType::NodeIterator it = rLocalNodes.begin(); it != rLocalNodes.end(); it++)
            {
                const double current_pr = it->FastGetSolutionStepValue(PRESSURE) / it->FastGetSolutionStepValue(DENSITY);
                reference_pr_norm += current_pr*current_pr;

                const double delta_pr = current_pr - it->GetValue(PRESSURE);
                difference_pr_norm += delta_pr*delta_pr;

                const array_1d<double,3> current_vel = it->FastGetSolutionStepValue(VELOCITY);
                reference_vel_norm += current_vel[0]*current_vel[0] + current_vel[1]*current_vel[1] + current_vel[2]*current_vel[2];

                const array_1d<double,3> delta_vel = current_vel - it->GetValue(VELOCITY);
                difference_vel_norm += delta_vel[0]*delta_vel[0] + delta_vel[1]*delta_vel[1] + delta_vel[2]*delta_vel[2];
            }

            double nnode = double(r_model_part.GetCommunicator().LocalMesh().NumberOfNodes());
            double sendbuff[5] = { reference_vel_norm, difference_vel_norm, reference_pr_norm, difference_pr_norm, nnode };
            double recvbuff[5];

            // Get a reference to the MPI communicator (Epetra does not provide a direct interface to MPI_Reduce)
            const MPI_Comm& rComm = mrComm.Comm();

            MPI_Reduce(&sendbuff,&recvbuff,5,MPI_DOUBLE,MPI_SUM,0,rComm);

            int converged = 0;

            if (mrComm.MyPID() == 0)
            {
                const double dimension = (r_model_part.ElementsBegin()->GetGeometry()).WorkingSpaceDimension();
                const double vel_norm = sqrt( recvbuff[0] );
                const double vel_diff_norm = sqrt( recvbuff[1] );
                const double pr_norm = sqrt( recvbuff[2] );
                const double pr_diff_norm = sqrt( recvbuff[3] );
                const double global_nnode = recvbuff[4];

                const double vel_ratio = vel_diff_norm / vel_norm;
                const double vel_abs = vel_diff_norm / (dimension*global_nnode);

                const double pr_ratio = pr_diff_norm / pr_norm;
                const double pr_abs = pr_diff_norm / global_nnode;

                std::cout << "VELOCITY error (tolerance) -- Ratio: " << vel_ratio << " (" << mVelRatioTolerance << "); Absolute: " <<  vel_abs << " (" << mVelAbsTolerance << ")." << std::endl;
                std::cout << "PRESSURE error (tolerance) -- Ratio: " <<  pr_ratio << " (" << mPrsRatioTolerance << "); Absolute: " <<   pr_abs << " (" << mPrsAbsTolerance << ")." << std::endl;

                if( (vel_ratio <= mVelRatioTolerance || vel_abs<mVelAbsTolerance) && (pr_ratio <= mPrsRatioTolerance || pr_abs<mPrsAbsTolerance) )
                {
                    converged = 1;
                    std::cout << "-- Convergence in achieved --" << std::endl;
                }
            }

            // Broadcast convergence status from process 0
            MPI_Bcast(&converged,1,MPI_INT,0,rComm);

            return bool(converged);

        }
        else // if size(Dx) == 0 there are no unknowns to solve for!
        {
            return true;
        }
    }

    void Initialize(
        ModelPart& r_model_part
    )
    {
        BaseType::mConvergenceCriteriaIsInitialized = true;
    }

    void InitializeSolutionStep(
        ModelPart& r_model_part,
        DofsArrayType& rDofSet,
        const TSystemMatrixType& A,
        const TSystemVectorType& Dx,
        const TSystemVectorType& b
    )
    {
    }

    void FinalizeSolutionStep(
        ModelPart& r_model_part,
        DofsArrayType& rDofSet,
        const TSystemMatrixType& A,
        const TSystemVectorType& Dx,
        const TSystemVectorType& b
    ) {}



    /*@} */
    /**@name Operations */
    /*@{ */


    /*@} */
    /**@name Access */
    /*@{ */


    /*@} */
    /**@name Inquiry */
    /*@{ */


    /*@} */
    /**@name Friends */
    /*@{ */


    /*@} */

protected:
    /**@name Protected static Member Variables */
    /*@{ */


    /*@} */
    /**@name Protected member Variables */
    /*@{ */


    /*@} */
    /**@name Protected Operators*/
    /*@{ */


    /*@} */
    /**@name Protected Operations*/
    /*@{ */


    /*@} */
    /**@name Protected  Access */
    /*@{ */


    /*@} */
    /**@name Protected Inquiry */
    /*@{ */


    /*@} */
    /**@name Protected LifeCycle */
    /*@{ */



    /*@} */

private:
    /**@name Static Member Variables */
    /*@{ */


    /*@} */
    /**@name Member Variables */
    /*@{ */
    TDataType mVelRatioTolerance;
    TDataType mVelAbsTolerance;

    TDataType mPrsRatioTolerance;
    TDataType mPrsAbsTolerance;


    Epetra_MpiComm& mrComm;


    /*@} */
    /**@name Private Operations*/
    /*@{ */


    /*@} */
    /**@name Private  Access */
    /*@{ */


    /*@} */
    /**@name Private Inquiry */
    /*@{ */


    /*@} */
    /**@name Un accessible methods */
    /*@{ */


    /*@} */

}; /* Class ClassName */

/*@} */

/**@name Type Definitions */
/*@{ */


/*@} */

}  /* namespace Kratos.*/

#endif /* KRATOS_TRILINOS_DISPLACEMENT_CRITERIA  defined */


//        
// Author: Miguel AngelCeligueta, maceli@cimne.upc.edu
//

//#include "DEM_application.h"
#include "custom_utilities/GeometryFunctions.h"
#include "custom_elements/cluster3D.h"
#include "dem_integration_scheme.h"
#include "DEM_application_variables.h"


namespace Kratos {

    DEMIntegrationScheme::DEMIntegrationScheme(){}
    DEMIntegrationScheme::~DEMIntegrationScheme(){}

    void DEMIntegrationScheme::AddSpheresVariables(ModelPart & r_model_part){
        
        r_model_part.AddNodalSolutionStepVariable(VELOCITY);
        r_model_part.AddNodalSolutionStepVariable(DISPLACEMENT);
        r_model_part.AddNodalSolutionStepVariable(DELTA_DISPLACEMENT);
        r_model_part.AddNodalSolutionStepVariable(TOTAL_FORCES);
        r_model_part.AddNodalSolutionStepVariable(NODAL_MASS);   
        
        if (mRotationOption){
            r_model_part.AddNodalSolutionStepVariable(PARTICLE_MOMENT_OF_INERTIA); 
            r_model_part.AddNodalSolutionStepVariable(ANGULAR_VELOCITY); 
            r_model_part.AddNodalSolutionStepVariable(PARTICLE_MOMENT); 
            r_model_part.AddNodalSolutionStepVariable(PARTICLE_ROTATION_ANGLE); 
            r_model_part.AddNodalSolutionStepVariable(DELTA_ROTATION);         
        }
    }
    
    void DEMIntegrationScheme::AddClustersVariables(ModelPart & r_model_part){
        
        r_model_part.AddNodalSolutionStepVariable(VELOCITY);
        r_model_part.AddNodalSolutionStepVariable(DISPLACEMENT);
        r_model_part.AddNodalSolutionStepVariable(DELTA_DISPLACEMENT);
        r_model_part.AddNodalSolutionStepVariable(TOTAL_FORCES);
        r_model_part.AddNodalSolutionStepVariable(NODAL_MASS);     
        
        if (mRotationOption){
            r_model_part.AddNodalSolutionStepVariable(PRINCIPAL_MOMENTS_OF_INERTIA); 
            r_model_part.AddNodalSolutionStepVariable(ANGULAR_VELOCITY); 
            r_model_part.AddNodalSolutionStepVariable(PARTICLE_MOMENT); 
            r_model_part.AddNodalSolutionStepVariable(PARTICLE_ROTATION_ANGLE); 
            r_model_part.AddNodalSolutionStepVariable(EULER_ANGLES); 
            r_model_part.AddNodalSolutionStepVariable(ORIENTATION);
            r_model_part.AddNodalSolutionStepVariable(DELTA_ROTATION);   
        }
    }
    
    void DEMIntegrationScheme::SetRotationOption(const int rotation_option) {
        if(rotation_option) mRotationOption = true;
        else mRotationOption = false;
    }

    void DEMIntegrationScheme::UpdateLinearDisplacementAndVelocityOfSpheres(ModelPart & rcluster_model_part) { //must be done AFTER the translational motion!

        KRATOS_TRY

        typedef ModelPart::ElementsContainerType ElementsArrayType;
        typedef ElementsArrayType::iterator ElementIterator;

        vector<unsigned int> element_partition;
        ElementsArrayType& pElements = rcluster_model_part.GetCommunicator().LocalMesh().Elements();
        OpenMPUtils::CreatePartition(OpenMPUtils::GetNumThreads(), pElements.size(), element_partition);

        #pragma omp parallel for
        for (int k = 0; k < (int) OpenMPUtils::GetNumThreads(); k++) {
            ElementIterator i_begin = pElements.ptr_begin() + element_partition[k];
            ElementIterator i_end = pElements.ptr_begin() + element_partition[k + 1];

            for (ElementsArrayType::iterator it = i_begin; it != i_end; ++it) {
                Cluster3D& cluster_element = dynamic_cast<Kratos::Cluster3D&> (*it);
                cluster_element.UpdateLinearDisplacementAndVelocityOfSpheres();
            }
        }
        KRATOS_CATCH(" ")
    }
    
    void DEMIntegrationScheme::Calculate(ModelPart& model_part, int StepFlag) {
        KRATOS_TRY
        ProcessInfo& r_process_info = model_part.GetProcessInfo();
        NodesArrayType& pLocalNodes = model_part.GetCommunicator().LocalMesh().Nodes();
        NodesArrayType& pGhostNodes = model_part.GetCommunicator().GhostMesh().Nodes();
        CalculateTranslationalMotion(model_part, pLocalNodes, StepFlag);
        CalculateTranslationalMotion(model_part, pGhostNodes, StepFlag);

        if (!r_process_info[CONTAINS_CLUSTERS]) {
            if (mRotationOption) {
                CalculateRotationalMotion(model_part, pLocalNodes, StepFlag);
                CalculateRotationalMotion(model_part, pGhostNodes, StepFlag);
            }
        } else {

            if (!mRotationOption) {
                UpdateLinearDisplacementAndVelocityOfSpheres(model_part);
            } else {
                CalculateRotationalMotionOfClusters(model_part, StepFlag);
            }
        }
        KRATOS_CATCH(" ")
    }
        
    void DEMIntegrationScheme::UpdateTranslationalVariables(
                            int StepFlag,
                            const Node < 3 > & i,
                            array_1d<double, 3 >& coor,
                            array_1d<double, 3 >& displ,
                            array_1d<double, 3 >& delta_displ,
                            array_1d<double, 3 >& vel,
                            const array_1d<double, 3 >& initial_coor,
                            const array_1d<double, 3 >& force,
                            const double force_reduction_factor,
                            const double mass,
                            const double delta_t,
                            const bool Fix_vel[3]) {

        KRATOS_THROW_ERROR(std::runtime_error, "This function (DEMIntegrationScheme::UpdateTranslationalVariables) shouldn't be accessed, use derived class instead", 0);
    }
    
    void DEMIntegrationScheme::CalculateTranslationalMotion(ModelPart& model_part, NodesArrayType& pNodes, int StepFlag) {
        KRATOS_TRY
        ProcessInfo& r_process_info = model_part.GetProcessInfo();
        double delta_t = r_process_info[DELTA_TIME];
        double virtual_mass_coeff = r_process_info[NODAL_MASS_COEFF]; //TODO: change the name of this variable to FORCE_REDUCTION_FACTOR
        bool virtual_mass_option = (bool) r_process_info[VIRTUAL_MASS_OPTION];
        double force_reduction_factor = 1.0;
        if (virtual_mass_option) {
            force_reduction_factor = virtual_mass_coeff;
            if ((force_reduction_factor > 1.0) || (force_reduction_factor < 0.0)) {
                KRATOS_THROW_ERROR(std::runtime_error, "The force reduction factor is either larger than 1 or negative: FORCE_REDUCTION_FACTOR= ", virtual_mass_coeff)
            }
        }                        

        #pragma omp parallel for shared(delta_t)
        for (int k = 0; k < (int) pNodes.size(); k++) {
            ModelPart::NodeIterator i_iterator = pNodes.ptr_begin() + k;
            Node < 3 > & i = *i_iterator;
            if (i.Is(DEMFlags::BELONGS_TO_A_CLUSTER)) continue;
            array_1d<double, 3 >& vel = i.FastGetSolutionStepValue(VELOCITY);
            array_1d<double, 3 >& displ = i.FastGetSolutionStepValue(DISPLACEMENT);
            array_1d<double, 3 >& delta_displ = i.FastGetSolutionStepValue(DELTA_DISPLACEMENT);
            array_1d<double, 3 >& coor = i.Coordinates();
            array_1d<double, 3 >& initial_coor = i.GetInitialPosition();
            array_1d<double, 3 >& force = i.FastGetSolutionStepValue(TOTAL_FORCES);

            double mass = i.FastGetSolutionStepValue(NODAL_MASS);                   

            bool Fix_vel[3] = {false, false, false};

            Fix_vel[0] = i.Is(DEMFlags::FIXED_VEL_X);
            Fix_vel[1] = i.Is(DEMFlags::FIXED_VEL_Y);
            Fix_vel[2] = i.Is(DEMFlags::FIXED_VEL_Z);

            UpdateTranslationalVariables(StepFlag, i, coor, displ, delta_displ, vel, initial_coor, force, force_reduction_factor, mass, delta_t, Fix_vel);

        } //nodes in the thread
        KRATOS_CATCH(" ")
    }
    
    void DEMIntegrationScheme::CalculateLocalAngularAcceleration(
                                const Node < 3 > & i,
                                const double moment_of_inertia,
                                const array_1d<double, 3 >& torque, 
                                const double moment_reduction_factor,
                                array_1d<double, 3 >& angular_acceleration){
        KRATOS_THROW_ERROR(std::runtime_error, "This function (DEMIntegrationScheme::CalculateLocalAngularAcceleration) shouldn't be accessed, use derived class instead", 0);            
    }
    
    void DEMIntegrationScheme::UpdateRotationalVariables(
                int StepFlag,
                const Node < 3 > & i,
                array_1d<double, 3 >& rotated_angle,
                array_1d<double, 3 >& delta_rotation,
                array_1d<double, 3 >& angular_velocity,
                array_1d<double, 3 >& angular_acceleration,
                const double delta_t,
                const bool Fix_Ang_vel[3]) {

        KRATOS_THROW_ERROR(std::runtime_error, "This function (DEMIntegrationScheme::UpdateRotationalVariables) shouldn't be accessed, use derived class instead", 0);
    }   
    
    void DEMIntegrationScheme::CalculateRotationalMotion(ModelPart& model_part, NodesArrayType& pNodes, int StepFlag) {

        KRATOS_TRY

        ProcessInfo& r_process_info = model_part.GetProcessInfo();
        double delta_t = r_process_info[DELTA_TIME];
        vector<unsigned int> node_partition;
        double virtual_mass_coeff = r_process_info[NODAL_MASS_COEFF];
        bool virtual_mass_option = (bool) r_process_info[VIRTUAL_MASS_OPTION];
        double moment_reduction_factor = 1.0;
        if (virtual_mass_option) {
            moment_reduction_factor = 1.0 - virtual_mass_coeff;
            if ((moment_reduction_factor > 1.0) || (moment_reduction_factor < 0.0)) {
                KRATOS_THROW_ERROR(std::runtime_error, "The virtual mass coefficient is either larger than 1 or negative: virtual_mass_coeff= ", virtual_mass_coeff)
            }
        }      

        #pragma omp parallel for shared(delta_t)
        for (int k = 0; k < (int) pNodes.size(); k++) {
            ModelPart::NodeIterator i_iterator = pNodes.ptr_begin() + k;
            Node < 3 > & i = *i_iterator;

            double moment_of_inertia = i.FastGetSolutionStepValue(PARTICLE_MOMENT_OF_INERTIA);

            array_1d<double, 3 >& angular_velocity = i.FastGetSolutionStepValue(ANGULAR_VELOCITY);
            array_1d<double, 3 >& torque = i.FastGetSolutionStepValue(PARTICLE_MOMENT);
            array_1d<double, 3 >& rotated_angle = i.FastGetSolutionStepValue(PARTICLE_ROTATION_ANGLE);
            array_1d<double, 3 >& delta_rotation = i.FastGetSolutionStepValue(DELTA_ROTATION);
                      
            bool Fix_Ang_vel[3] = {false, false, false};

            Fix_Ang_vel[0] = i.Is(DEMFlags::FIXED_ANG_VEL_X);
            Fix_Ang_vel[1] = i.Is(DEMFlags::FIXED_ANG_VEL_Y);
            Fix_Ang_vel[2] = i.Is(DEMFlags::FIXED_ANG_VEL_Z);

            array_1d<double, 3 > angular_acceleration;                    
            CalculateLocalAngularAcceleration(i, moment_of_inertia, torque, moment_reduction_factor,angular_acceleration);

            UpdateRotationalVariables(StepFlag, i, rotated_angle, delta_rotation, angular_velocity, angular_acceleration, delta_t, Fix_Ang_vel);
        }//for Node                  

        KRATOS_CATCH(" ")

    }//rotational_motion
    
    void DEMIntegrationScheme::CalculateLocalAngularAccelerationByEulerEquations(
                                    const Node < 3 > & i,
                                    const array_1d<double, 3 >& local_angular_velocity,
                                    const array_1d<double, 3 >& moments_of_inertia,
                                    const array_1d<double, 3 >& local_torque, 
                                    const double moment_reduction_factor,
                                    array_1d<double, 3 >& local_angular_acceleration){
            KRATOS_THROW_ERROR(std::runtime_error, "This function (DEMIntegrationScheme::CalculateLocalAngularAccelerationByEulerEquations) shouldn't be accessed, use derived class instead", 0);                        
    }   
    
    void DEMIntegrationScheme::CalculateRotationalMotionOfClusters(ModelPart& rcluster_model_part, int StepFlag) { //must be done AFTER the translational motion!

        KRATOS_TRY

        typedef ModelPart::ElementsContainerType ElementsArrayType;
        typedef ElementsArrayType::iterator ElementIterator;
        ProcessInfo& r_process_info = rcluster_model_part.GetProcessInfo();

        double delta_t = r_process_info[DELTA_TIME];
        double virtual_mass_coeff = r_process_info[NODAL_MASS_COEFF];
        bool virtual_mass_option = (bool) r_process_info[VIRTUAL_MASS_OPTION];
        double moment_reduction_factor = 1.0;
        if (virtual_mass_option) {
            moment_reduction_factor = 1.0 - virtual_mass_coeff;
            if ((moment_reduction_factor > 1.0) || (moment_reduction_factor < 0.0)) {
                KRATOS_THROW_ERROR(std::runtime_error, "The virtual mass coefficient is either larger than 1 or negative: virtual_mass_coeff= ", virtual_mass_coeff)
            }
        }     

        vector<unsigned int> element_partition;
        ElementsArrayType& pElements = rcluster_model_part.GetCommunicator().LocalMesh().Elements();
        OpenMPUtils::CreatePartition(OpenMPUtils::GetNumThreads(), pElements.size(), element_partition);

        #pragma omp parallel for shared(delta_t)        
        for (int k = 0; k < (int) OpenMPUtils::GetNumThreads(); k++) {
            ElementIterator i_begin = pElements.ptr_begin() + element_partition[k];
            ElementIterator i_end = pElements.ptr_begin() + element_partition[k + 1];

            for (ElementsArrayType::iterator it = i_begin; it != i_end; ++it) {
                Cluster3D& cluster_element = dynamic_cast<Kratos::Cluster3D&> (*it);
                Node < 3 > & i = cluster_element.GetGeometry()[0];

                array_1d<double, 3 > & moments_of_inertia = i.FastGetSolutionStepValue(PRINCIPAL_MOMENTS_OF_INERTIA);
                array_1d<double, 3 > & angular_velocity = i.FastGetSolutionStepValue(ANGULAR_VELOCITY);
                array_1d<double, 3 > & torque = i.FastGetSolutionStepValue(PARTICLE_MOMENT);
                array_1d<double, 3 > & rotated_angle = i.FastGetSolutionStepValue(PARTICLE_ROTATION_ANGLE);
                Quaternion<double  > & Orientation = i.FastGetSolutionStepValue(ORIENTATION);
                array_1d<double, 3 > & delta_rotation = i.FastGetSolutionStepValue(DELTA_ROTATION);

                array_1d<double, 3 > local_angular_velocity, local_angular_acceleration, local_torque, angular_acceleration;

                //Angular velocity and torques are saved in the local framework:
                GeometryFunctions::QuaternionVectorGlobal2Local(Orientation, torque, local_torque);
                GeometryFunctions::QuaternionVectorGlobal2Local(Orientation, angular_velocity, local_angular_velocity);
                CalculateLocalAngularAccelerationByEulerEquations(i,local_angular_velocity,moments_of_inertia,local_torque, moment_reduction_factor,local_angular_acceleration);                        

                //Angular acceleration is saved in the Global framework:
                GeometryFunctions::QuaternionVectorLocal2Global(Orientation, local_angular_acceleration, angular_acceleration);

                bool Fix_Ang_vel[3] = {false, false, false};

                Fix_Ang_vel[0] = i.Is(DEMFlags::FIXED_ANG_VEL_X);
                Fix_Ang_vel[1] = i.Is(DEMFlags::FIXED_ANG_VEL_Y);
                Fix_Ang_vel[2] = i.Is(DEMFlags::FIXED_ANG_VEL_Z);

                UpdateRotationalVariables(StepFlag, i, rotated_angle, delta_rotation, angular_velocity, angular_acceleration, delta_t, Fix_Ang_vel);                                               

                double ang = DEM_MODULUS_3(delta_rotation);
                if (ang) {
                    array_1d<double, 3 > & EulerAngles = i.FastGetSolutionStepValue(EULER_ANGLES);
                    Quaternion<double> delta_orientation = Quaternion<double>::Identity();
                        
                    array_1d<double, 3 > theta = delta_rotation;
                    DEM_MULTIPLY_BY_SCALAR_3(theta, 0.5);

                    double thetaMag = DEM_MODULUS_3(theta);
					const double epsilon = std::numeric_limits<double>::epsilon();
					if (thetaMag * thetaMag * thetaMag * thetaMag / 24.0 < epsilon) { //Taylor: low angle
                        double aux = (1 - thetaMag * thetaMag / 6);
                        delta_orientation = Quaternion<double>((1 + thetaMag * thetaMag / 2), theta[0]*aux, theta[1]*aux, theta[2]*aux);
                    }
                    else {
                        double aux = sin(thetaMag)/thetaMag;
                        delta_orientation = Quaternion<double>(cos(thetaMag), theta[0]*aux, theta[1]*aux, theta[2]*aux);
                    }

                    delta_orientation.normalize();
                    Orientation = delta_orientation * Orientation;
                    Orientation.ToEulerAngles(EulerAngles); 
                } //if ang                                                                                                    
                cluster_element.UpdatePositionOfSpheres();
            } //for Elements
        } //for number of threads
        KRATOS_CATCH(" ")
    }
}

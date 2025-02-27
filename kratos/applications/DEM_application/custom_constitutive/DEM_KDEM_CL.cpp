// System includes
#include <string>
#include <iostream>
#include <cmath>

// Project includes
//#include "DEM_application.h"
#include "DEM_KDEM_CL.h"
//#include "custom_elements/spheric_particle.h"
#include "custom_elements/spheric_continuum_particle.h"

namespace Kratos {

    void DEM_KDEM::Initialize() {

        KRATOS_TRY
        KRATOS_CATCH("")  
    }

    DEMContinuumConstitutiveLaw::Pointer DEM_KDEM::Clone() const {
        DEMContinuumConstitutiveLaw::Pointer p_clone(new DEM_KDEM(*this));
        return p_clone;
    }

    void DEM_KDEM::SetConstitutiveLawInProperties(Properties::Pointer pProp) const {
        std::cout << "\nAssigning DEM_KDEM to Properties " << pProp->Id() << std::endl;
        pProp->SetValue(DEM_CONTINUUM_CONSTITUTIVE_LAW_POINTER, this->Clone());
    }

    void DEM_KDEM::CalculateContactArea(double radius, double other_radius, double& calculation_area) {

        KRATOS_TRY
        double radius_sum = radius + other_radius;
        double equiv_radius = radius * other_radius / radius_sum;
        calculation_area = KRATOS_M_PI * equiv_radius * equiv_radius;
        KRATOS_CATCH("")  
    }
    
    double DEM_KDEM::CalculateContactArea(double radius, double other_radius, std::vector<double>& v) {            
        double a = 0.0;
        CalculateContactArea(radius, other_radius, a);
        v.push_back(a);
        return a;
    }
    
    void DEM_KDEM::GetContactArea(const double radius, const double other_radius, const std::vector<double> & vector_of_initial_areas, const int neighbour_position, double& calculation_area) {
        if (vector_of_initial_areas.size()) calculation_area = vector_of_initial_areas[neighbour_position];
        else CalculateContactArea(radius, other_radius, calculation_area);
    }

    void DEM_KDEM::CalculateElasticConstants(double& kn_el, double& kt_el, double initial_dist, double equiv_young,
                                             double equiv_poisson, double calculation_area) {
        
        KRATOS_TRY
        double equiv_shear = equiv_young / (2.0 * (1 + equiv_poisson)); // TODO: Is this correct? SLS
        kn_el = equiv_young * calculation_area / initial_dist;
        kt_el = equiv_shear * calculation_area / initial_dist;
        KRATOS_CATCH("")  
    }
                        
    void DEM_KDEM::CalculateViscoDampingCoeff(double& equiv_visco_damp_coeff_normal,
                                              double& equiv_visco_damp_coeff_tangential,
                                              SphericContinuumParticle* element1,
                                              SphericContinuumParticle* element2,
                                              const double kn_el,
                                              const double kt_el) {
        
        KRATOS_TRY
        
        double aux_norm_to_tang = sqrt(kt_el / kn_el);
        const double mRealMass = element1->GetMass();
        const double other_real_mass = element2->GetMass();
        const double mCoefficientOfRestitution = element1->GetCoefficientOfRestitution();
        const double mOtherCoefficientOfRestitution = element2->GetCoefficientOfRestitution();
        const double equiv_coefficientOfRestitution = 0.5 * (mCoefficientOfRestitution + mOtherCoefficientOfRestitution);

        equiv_visco_damp_coeff_normal = (1.0 - equiv_coefficientOfRestitution) * 2.0 * sqrt(kn_el / (mRealMass + other_real_mass)) * (sqrt(mRealMass * other_real_mass)); // := 2d0* sqrt ( kn_el*(m1*m2)/(m1+m2) )
        equiv_visco_damp_coeff_tangential = equiv_visco_damp_coeff_normal * aux_norm_to_tang;
        
        KRATOS_CATCH("")  
    }
    


    double DEM_KDEM::LocalMaxSearchDistance(const int i,
                                            SphericContinuumParticle* element1,
                                            SphericContinuumParticle* element2) {

        Properties& element1_props = element1->GetProperties();
        Properties& element2_props = element2->GetProperties();
        double mTensionLimit;

        // calculation of equivalent young modulus
        double myYoung = element1->GetYoung();
        double other_young = element2->GetYoung();
        double equiv_young = 2.0 * myYoung * other_young / (myYoung + other_young);

        const double my_radius = element1->GetRadius();
        const double other_radius = element2->GetRadius();
        double calculation_area = 0;

        CalculateContactArea(my_radius, other_radius, calculation_area);

        double radius_sum = my_radius + other_radius;
        double initial_delta = element1->GetInitialDelta(i);
        double initial_dist = radius_sum - initial_delta;

        // calculation of elastic constants
        double kn_el = equiv_young * calculation_area / initial_dist;

        if (&element1_props == &element2_props) {
             mTensionLimit = element1_props[CONTACT_SIGMA_MIN]*1e6;
        } else {
            mTensionLimit = 0.5*1e6*(element1_props[CONTACT_SIGMA_MIN] + element2_props[CONTACT_SIGMA_MIN]);
        }

        const double Ntstr_el = mTensionLimit * calculation_area;
        double u1 = Ntstr_el / kn_el;
        if (u1 > 2*radius_sum) {u1 = 2*radius_sum;}   // avoid error in special cases with too high tensile
        return u1;

    }


    void DEM_KDEM::CalculateForces(const ProcessInfo& r_process_info,
                                double OldLocalElasticContactForce[3],
                                double LocalElasticContactForce[3],
                                double LocalDeltDisp[3],
                                const double kn_el,
                                const double kt_el,
                                double& contact_sigma,
                                double& contact_tau,
                                double& failure_criterion_state,
                                double equiv_young,
                                double indentation,
                                double calculation_area,
                                double& acumulated_damage,
                                SphericContinuumParticle* element1,
                                SphericContinuumParticle* element2,
                                int i_neighbour_count,
                                int time_steps,
                                bool& sliding,
                                int search_control,
                                vector<int>& search_control_vector,
                                double &equiv_visco_damp_coeff_normal,
                                double &equiv_visco_damp_coeff_tangential,
                                double LocalRelVel[3],
                                double ViscoDampingLocalContactForce[3],
                                int failure_id) {
                
        KRATOS_TRY
        CalculateNormalForces(LocalElasticContactForce,
                kn_el,
                equiv_young,
                indentation,
                calculation_area,
                acumulated_damage,
                element1,
                element2,
                i_neighbour_count,
                time_steps);
        



        CalculateTangentialForces(LocalElasticContactForce,
                LocalDeltDisp,
                kt_el,
                contact_sigma,
                contact_tau,
                indentation,
                calculation_area,
                failure_criterion_state,
                element1,
                element2,
                i_neighbour_count,
                sliding,
                search_control,
                search_control_vector);

        CalculateViscoDampingCoeff(equiv_visco_damp_coeff_normal,
                                   equiv_visco_damp_coeff_tangential,
                                   element1,
                                   element2,
                                   kn_el,
                                   kt_el);

        CalculateViscoDamping(LocalRelVel,
                              ViscoDampingLocalContactForce,
                              indentation,
                              equiv_visco_damp_coeff_normal,
                              equiv_visco_damp_coeff_tangential,
                              sliding,
                              failure_id);
        
    KRATOS_CATCH("")      
    }

    void DEM_KDEM::CalculateNormalForces(double LocalElasticContactForce[3],
            const double kn_el,
            double equiv_young,
            double indentation,
            double calculation_area,
            double& acumulated_damage,
            SphericContinuumParticle* element1,
            SphericContinuumParticle* element2,
            int i_neighbour_count,
            int time_steps) {

        KRATOS_TRY       
        
        if (indentation >= 0.0) { //COMPRESSION
            LocalElasticContactForce[2] = kn_el * indentation;  
        }
        else { //tension   
            int& failure_type = element1->mIniNeighbourFailureId[i_neighbour_count];
            if (failure_type == 0) {  
                double mTensionLimit = 0.5 * 1e6 * (element1->GetProperties()[CONTACT_SIGMA_MIN] + element2->GetProperties()[CONTACT_SIGMA_MIN]); //N/m2
                const double limit_force = mTensionLimit * calculation_area;
                LocalElasticContactForce[2] = kn_el * indentation;
                if (fabs(LocalElasticContactForce[2]) > limit_force) {          
                    failure_type = 4; //tension failure
                    LocalElasticContactForce[2] = 0.0;
                }                
            } 
            else {
                LocalElasticContactForce[2] = 0.0; 
            }
        }
        KRATOS_CATCH("")      
    }



    void DEM_KDEM::CalculateTangentialForces(double LocalElasticContactForce[3],
            double LocalDeltDisp[3],
            const double kt_el,
            double& contact_sigma,
            double& contact_tau,
            double indentation,
            double calculation_area,
            double& failure_criterion_state,
            SphericContinuumParticle* element1,
            SphericContinuumParticle* element2,
            int i_neighbour_count,
            bool& sliding,
            int search_control,
            vector<int>& search_control_vector) {



        KRATOS_TRY

        int& failure_type = element1->mIniNeighbourFailureId[i_neighbour_count];                
        LocalElasticContactForce[0] -= kt_el * LocalDeltDisp[0]; // 0: first tangential
        LocalElasticContactForce[1] -= kt_el * LocalDeltDisp[1]; // 1: second tangential
        
        double ShearForceNow = sqrt(LocalElasticContactForce[0] * LocalElasticContactForce[0]
                                  + LocalElasticContactForce[1] * LocalElasticContactForce[1]);
            
        if (failure_type == 0) { //This means it has not broken 
            Properties& element1_props = element1->GetProperties();
            Properties& element2_props = element2->GetProperties();
            const double mTauZero = 0.5 * 1e6 * (element1_props[CONTACT_TAU_ZERO] + element2_props[CONTACT_TAU_ZERO]);
            const double mInternalFriction = 0.5 * (element1_props[CONTACT_INTERNAL_FRICC] + element2_props[CONTACT_INTERNAL_FRICC]);

            contact_tau = ShearForceNow / calculation_area;
            contact_sigma = LocalElasticContactForce[2] / calculation_area;

            double tau_strength = mTauZero;

            if (contact_sigma >= 0) {
                tau_strength = mTauZero + mInternalFriction * contact_sigma;
            }

            if (contact_tau > tau_strength) {                                
                failure_type = 2; // shear
            }
        }
        else {
            const double equiv_tg_of_fri_ang = 0.5 * (element1->GetTgOfFrictionAngle() + element2->GetTgOfFrictionAngle());  
            double Frictional_ShearForceMax = equiv_tg_of_fri_ang * LocalElasticContactForce[2];
                
            if (Frictional_ShearForceMax < 0.0) {
                Frictional_ShearForceMax = 0.0;
            }

            if ((ShearForceNow > Frictional_ShearForceMax) && (ShearForceNow != 0.0)) {
                LocalElasticContactForce[0] = (Frictional_ShearForceMax / ShearForceNow) * LocalElasticContactForce[0];
                LocalElasticContactForce[1] = (Frictional_ShearForceMax / ShearForceNow) * LocalElasticContactForce[1];
                sliding = true;
            }
        }

        KRATOS_CATCH("")      
    }
    
    void DEM_KDEM::CalculateViscoDamping(double LocalRelVel[3],
                                         double ViscoDampingLocalContactForce[3],
                                         double indentation,
                                         double equiv_visco_damp_coeff_normal,
                                         double equiv_visco_damp_coeff_tangential,
                                         bool& sliding,
                                         int failure_id) {

        KRATOS_TRY  
                
        if ((indentation > 0) || (failure_id == 0)) {
            ViscoDampingLocalContactForce[2] = -equiv_visco_damp_coeff_normal * LocalRelVel[2];
        }
        
        if (((indentation > 0) || (failure_id == 0)) && (sliding == false)) {    
            ViscoDampingLocalContactForce[0] = -equiv_visco_damp_coeff_tangential * LocalRelVel[0];
            ViscoDampingLocalContactForce[1] = -equiv_visco_damp_coeff_tangential * LocalRelVel[1];
        }
        
        KRATOS_CATCH("")
    }
    
    void DEM_KDEM::ComputeParticleRotationalMoments(SphericContinuumParticle* element,
                                                    SphericContinuumParticle* neighbor,
                                                    double equiv_young,
                                                    double distance,
                                                    double calculation_area,
                                                    double LocalCoordSystem[3][3],
                                                    double ElasticLocalRotationalMoment[3],
                                                    double ViscoLocalRotationalMoment[3]) {

        KRATOS_TRY 
        //double LocalRotationalMoment[3]     = {0.0};
        double LocalDeltaRotatedAngle[3]    = {0.0};
        double LocalDeltaAngularVelocity[3] = {0.0};
        
        array_1d<double, 3> GlobalDeltaRotatedAngle;
        noalias(GlobalDeltaRotatedAngle) = element->GetGeometry()[0].FastGetSolutionStepValue(PARTICLE_ROTATION_ANGLE) - neighbor->GetGeometry()[0].FastGetSolutionStepValue(PARTICLE_ROTATION_ANGLE);
        array_1d<double, 3> GlobalDeltaAngularVelocity;
        noalias(GlobalDeltaAngularVelocity) = element->GetGeometry()[0].FastGetSolutionStepValue(ANGULAR_VELOCITY) - neighbor->GetGeometry()[0].FastGetSolutionStepValue(ANGULAR_VELOCITY);

        GeometryFunctions::VectorGlobal2Local(LocalCoordSystem, GlobalDeltaRotatedAngle, LocalDeltaRotatedAngle);
        GeometryFunctions::VectorGlobal2Local(LocalCoordSystem, GlobalDeltaAngularVelocity, LocalDeltaAngularVelocity);
        //GeometryFunctions::VectorGlobal2Local(LocalCoordSystem, mContactMoment, LocalRotationalMoment);
        
        const double equivalent_radius = sqrt(calculation_area / KRATOS_M_PI);
        const double Inertia_I = 0.25 * KRATOS_M_PI * equivalent_radius * equivalent_radius * equivalent_radius * equivalent_radius;
        const double Inertia_J = 2.0 * Inertia_I; // This is the polar inertia
        const double rot_k = 1.0; // Hardcoded only for testing purposes. Obviously, this parameter should be always 1.0
                        
        const double element_mass  = element->GetMass();
        const double neighbor_mass = neighbor->GetMass();
        const double equiv_mass    = element_mass * neighbor_mass / (element_mass + neighbor_mass);
        
        // Viscous parameter taken from J.S.Marshall, 'Discrete-element modeling of particle aerosol flows', section 4.3. Twisting resistance
        const double alpha = 0.9; // Hardcoded only for testing purposes. This value depends on the restitution coefficient and goes from 0.1 to 1.0
        const double visc_param = 0.5 * equivalent_radius * equivalent_radius * alpha * sqrt(1.33333333333333333 * equiv_mass * equiv_young * equivalent_radius);
             
        //equiv_young or G in torsor (LocalRotationalMoment[2]) ///////// TODO
        
        ElasticLocalRotationalMoment[0] = -rot_k * equiv_young * Inertia_I * LocalDeltaRotatedAngle[0] / distance;
        ElasticLocalRotationalMoment[1] = -rot_k * equiv_young * Inertia_I * LocalDeltaRotatedAngle[1] / distance;
        ElasticLocalRotationalMoment[2] = -rot_k * equiv_young * Inertia_J * LocalDeltaRotatedAngle[2] / distance;
        
        ViscoLocalRotationalMoment[0] = -visc_param * LocalDeltaAngularVelocity[0];
        ViscoLocalRotationalMoment[1] = -visc_param * LocalDeltaAngularVelocity[1];
        ViscoLocalRotationalMoment[2] = -visc_param * LocalDeltaAngularVelocity[2];
        
        // Judge if the rotation spring is broken or not
        /*
        double ForceN  = LocalElasticContactForce[2];
        double ForceS  = sqrt(LocalElasticContactForce[0] * LocalElasticContactForce[0] + LocalElasticContactForce[1] * LocalElasticContactForce[1]);
        double MomentS = sqrt(LocalRotaSpringMoment[0] * LocalRotaSpringMoment[0] + LocalRotaSpringMoment[1] * LocalRotaSpringMoment[1]);
        double MomentN = LocalRotaSpringMoment[2];
        // bending stress and axial stress add together, use edge of the bar will failure first
        double TensiMax = -ForceN / calculation_area + MomentS / Inertia_I * equiv_radius;
        double ShearMax =  ForceS / calculation_area + fabs(MomentN) / Inertia_J * equiv_radius;
        if (TensiMax > equiv_tension || ShearMax > equiv_cohesion) {
            mRotaSpringFailureType[i_neighbor_count] = 1;
            LocalRotaSpringMoment[0] = LocalRotaSpringMoment[1] = LocalRotaSpringMoment[2] = 0.0;
            //LocalRotaSpringMoment[1] = 0.0;
            //LocalRotaSpringMoment[2] = 0.0;
        }
        */
        //GeometryFunctions::VectorLocal2Global(LocalCoordSystem, LocalRotationalMoment, mContactMoment);
        KRATOS_CATCH("")
    }//ComputeParticleRotationalMoments
    
    void DEM_KDEM::AddPoissonContribution(const double equiv_poisson, double LocalCoordSystem[3][3], double& normal_force, 
                                          double calculation_area, Matrix* mSymmStressTensor, SphericContinuumParticle* element1, SphericContinuumParticle* element2) {
        double force[3];
        Matrix average_stress_tensor = ZeroMatrix(3,3);
        
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                average_stress_tensor(i,j) = 0.5 * ((*mSymmStressTensor)(i,j) + (*(element2->mSymmStressTensor))(i,j));
            }
        }

        for (int i = 0; i < 3; i++) {
            
            force[i] = (average_stress_tensor)(i,0) * LocalCoordSystem[0][0] +
                       (average_stress_tensor)(i,1) * LocalCoordSystem[0][1] +
                       (average_stress_tensor)(i,2) * LocalCoordSystem[0][2]; //StressTensor*unitaryNormal0
        }
        
        double sigma_x = force[0] * LocalCoordSystem[0][0] +
                         force[1] * LocalCoordSystem[0][1] +
                         force[2] * LocalCoordSystem[0][2]; // projection to normal to obtain value of the normal stress

        for (int i = 0; i < 3; i++) {
            
            force[i] = (average_stress_tensor)(i,0) * LocalCoordSystem[1][0] +
                       (average_stress_tensor)(i,1) * LocalCoordSystem[1][1] +
                       (average_stress_tensor)(i,2) * LocalCoordSystem[1][2]; //StressTensor*unitaryNormal1
        }
        
        double sigma_y = force[0] * LocalCoordSystem[1][0] +
                         force[1] * LocalCoordSystem[1][1] +
                         force[2] * LocalCoordSystem[1][2]; // projection to normal to obtain value of the normal stress

        double poisson_force = calculation_area * equiv_poisson * (sigma_x + sigma_y);

        normal_force -= poisson_force;
    
    } //AddPoissonContribution

} // namespace Kratos

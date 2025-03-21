#ifndef UTERINECONDUCTIVITYMODIFIER_HPP
#define UTERINECONDUCTIVITYMODIFIER_HPP

#include <iostream>
#include "Exception.hpp"
#include "AbstractTetrahedralMesh.hpp"
#include "DistributedTetrahedralMesh.hpp"
#include "AbstractConductivityModifier.hpp"

//this is taken from https://chaste.github.io/docs/user-tutorials/bidomainwithconductivitymodifier/
//hardcoded for 3 dimensions
/* class UterineConductivityModifier : public AbstractConductivityModifier<3,3>
{
private:

    c_matrix<double,3,3> mTensor;
    c_matrix<double,3,3> mSpecialMatrix;


    
public:
    //for linear distribution along z
    double linear_conduct_z_slope;
    double linear_conduct_max;
    AbstractTetrahedralMesh<3,3>* p_mesh;

    UterineConductivityModifier();
         : AbstractConductivityModifier<3,3>(),
          mSpecialMatrix( zero_matrix<double>(3,3) )
          {
              mSpecialMatrix(0,0) = 1.0;
              mSpecialMatrix(1,1) = 1.0;
              mSpecialMatrix(2,2) = 1.0;
          }
    c_matrix<double,3,3>& rCalculateModifiedConductivityTensor(unsigned elementIndex, 
        const c_matrix<double,3,3>& rOriginalConductivity, unsigned domainIndex)
    {

        //get this element's centroid -- used for calculating position-based variation
        Element<3,3>* p_element = (p_mesh->GetElement(elementIndex));
        c_vector<double, 3> cur_centroid = p_element->CalculateCentroid();

        for ( unsigned i=0; i<3; i++ )
        {
            //for some reason, chaste divides the conductances by the...elementIndex...so we multiply here
            mTensor(i,i) = elementIndex*(linear_conduct_z_slope*cur_centroid(2) + linear_conduct_max);
        } */
 

    //below stuff from chaste example
 /*        if (elementIndex == 0)
        {
            // For element 0 let's return the "special matrix", regardless of intra/extracellular.
            return mSpecialMatrix;
        }

        // Otherwise, we change the behaviour depending on the `domainIndex` (intra/extracellular).
        double domain_scaling;
        if (domainIndex == 0)
        {
            domain_scaling = 1.0; // domainIndex==0 implies intracellular
            //std::cout << "(uterineconductivitymodifier.hpp) domainIndex: " << domainIndex << std::endl;
        }
        else
        {
            domain_scaling = 1.5; // domainIndex==1 implies extracellular
        }

        // Modify the current conductivity according to some expression by running along the diagonal,
        // save to the "working memory", and return.
        for ( unsigned i=0; i<3; i++ )
        {
            mTensor(i,i) = domain_scaling*elementIndex*rOriginalConductivity(i,i);
        } */
/*          return mTensor;
    } 
};  */

//class GaussianConductivityModifier : public AbstractConductivityModifier<3,3> 
class UterineConductivityModifier : public AbstractConductivityModifier<3,3> 
{
private:

    c_matrix<double,3,3> mTensor;
    c_matrix<double,3,3> mSpecialMatrix;


    
public:
    //for gaussian distribution along z
    double gauss_z_center;
    double gauss_z_steep;
    double gauss_z_min;
    AbstractTetrahedralMesh<3,3>* p_mesh;

    //mTensor(0,0) = 0;

//    GaussianConductivityModifier()
    UterineConductivityModifier()
        : AbstractConductivityModifier<3,3>(),
          mSpecialMatrix( zero_matrix<double>(3,3) )
          {
              mSpecialMatrix(0,0) = 1.0;
              mSpecialMatrix(1,1) = 1.0;
              mSpecialMatrix(2,2) = 1.0;
          }    
    c_matrix<double,3,3>& rCalculateModifiedConductivityTensor(unsigned elementIndex, 
        const c_matrix<double,3,3>& rOriginalConductivity, unsigned domainIndex)
    {

        //get this element's centroid -- used for calculating position-based variation
        Element<3,3>* p_element = (p_mesh->GetElement(elementIndex));
        c_vector<double, 3> cur_centroid = p_element->CalculateCentroid();

        for ( unsigned i=0; i<3; i++ )
        {
            //for some reason, chaste divides the conductances by the...elementIndex...so we multiply here
            mTensor(i,i) = elementIndex*(gauss_z_min + exp(-gauss_z_steep*pow(cur_centroid(2)-gauss_z_center,2.0)));
        }          
        return mTensor;
    }
};  

#endif //UTERINECONDUCTIVITYMODIFIER_HPP

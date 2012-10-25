#ifndef PCAMODEL_H
#define PCAMODEL_H

#include <cstdio>
#include "face_recognition/BasicDataType.h"

#define MIN_FACE_WIDTH 64
#define MIN_FACE_HEIGHT 80

// weighted pca model
class PCAModel
{
public:
    int m_nPCADim; //pca dim
    int m_nRawDim; //original image dim
    double* m_pMeanVec; //mean feature vector
    double* m_pEigenVec; //pca projection matrix
    double* m_pWeightMat; //weight matrix for weighted pca
    double* m_pSVal; //eigen value of the pca model
    double* m_pInput;
    PCAModel();
    PCAModel(char* strModelPath);
    void LoadModel(char* strmodelpath);
    void ComputePCACoeff(byte* pInput, double* pOutput);
    ~PCAModel();
};
#endif // PCAMODEL_H

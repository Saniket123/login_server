#include "pcamodel.h"

PCAModel::PCAModel()
{
    m_nPCADim = 0;
    m_nRawDim = 0;
    m_pMeanVec = NULL;
    m_pEigenVec = NULL;
    m_pWeightMat = NULL;
    m_pSVal = NULL;
    m_pInput = NULL;
}

PCAModel::PCAModel(char* strModelPath)
{
    FILE* fp = fopen(strModelPath,"rb");

    fread(&m_nPCADim, sizeof(int), 1, fp);
    fread(&m_nRawDim, sizeof(int), 1, fp);

    m_pMeanVec  = new double[m_nRawDim];
    m_pSVal = new double[m_nPCADim];
    m_pEigenVec = new double[m_nPCADim*m_nRawDim];
    m_pWeightMat = new double[MIN_FACE_HEIGHT*MIN_FACE_WIDTH];
    m_pInput = new double[MIN_FACE_HEIGHT*MIN_FACE_WIDTH];

    fread(m_pMeanVec, sizeof(double), m_nRawDim, fp);
    fread(m_pEigenVec, sizeof(double), m_nPCADim*m_nRawDim, fp);
    fread(m_pWeightMat, sizeof(double), MIN_FACE_HEIGHT*MIN_FACE_WIDTH, fp);
    fread(m_pSVal, sizeof(double), m_nPCADim, fp);

    fclose(fp);
}

void PCAModel::LoadModel(char* strmodelpath)
{
    FILE* fp = fopen(strmodelpath,"rb");

    fread(&m_nPCADim, sizeof(int), 1, fp);
    fread(&m_nRawDim, sizeof(int), 1, fp);

    m_pMeanVec  = new double[m_nRawDim];
    m_pSVal = new double[m_nPCADim];
    m_pEigenVec = new double[m_nPCADim*m_nRawDim];
    m_pWeightMat = new double[MIN_FACE_HEIGHT*MIN_FACE_WIDTH];
    m_pInput = new double[MIN_FACE_HEIGHT*MIN_FACE_WIDTH];

    fread(m_pMeanVec, sizeof(double), m_nRawDim, fp);
    fread(m_pEigenVec, sizeof(double), m_nPCADim*m_nRawDim, fp);
    fread(m_pWeightMat, sizeof(double), MIN_FACE_HEIGHT*MIN_FACE_WIDTH, fp);
    fread(m_pSVal, sizeof(double), m_nPCADim, fp);
    fclose(fp);
}

void PCAModel::ComputePCACoeff(byte* pInput, double* pOutput)
{
    int i,j;
    for(i=0; i<m_nRawDim; i++)
    {
        m_pInput[i] = pInput[i] * m_pWeightMat[i];
        m_pInput[i] -= m_pMeanVec[i];
    }
    for(i=0; i<m_nPCADim; i++)
    {
        pOutput[i] = 0;
        for(j=0; j<m_nRawDim; j++)
        {
            pOutput[i] += m_pInput[j] * m_pEigenVec[i*m_nRawDim+j];
        }
        pOutput[i] = pOutput[i] / m_pSVal[i];
    }
}

PCAModel::~PCAModel()
{
    delete [] m_pMeanVec;
    delete [] m_pEigenVec;
    delete [] m_pWeightMat;
    delete [] m_pSVal;
    delete [] m_pInput;
}


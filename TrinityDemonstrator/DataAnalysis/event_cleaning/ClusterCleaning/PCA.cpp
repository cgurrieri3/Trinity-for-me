#include "PCA.h"
#include <iostream>


PCA::PCA(int features): numFeatures(features), matrix(0, features), covarianceMatrix(features) {}

// Add a new row to the dataset
void PCA::AddRow(const std::vector<double>& row) {
    if (row.size() != static_cast<size_t>(numFeatures)) {   //checks that the row has the right number of features
        std::cerr << "Error: Row size must match number of features!\n";
        return;
    }
    data.push_back(row);
}


// Compute the covariance matrix

// https://datascienceplus.com/understanding-the-covariance-matrix/
void PCA::ComputeCovarianceMatrix() {
    // Checks to make sure that the data actually has rows
    int n = data.size();
    if (n == 0) {
        std::cerr << "Error: No data added!\n";
        return;
    }

    matrix.ResizeTo(n, numFeatures);  // Resize temp TMatrix matrix that gets built on the data to allow easier computation using root libraries
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < numFeatures; ++j)
            matrix(i, j) = data[i][j]; // inputs all the data into the matrix

    // Compute mean for each column aka mean of each feature 
    TVectorD mean(numFeatures);
    for (int j = 0; j < numFeatures; ++j) {
        double sum = 0;
        for (int i = 0; i < n; ++i)
            sum += matrix(i, j);
        mean(j) = sum / n;
    }

    // Subtract mean for each column from each column
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < numFeatures; ++j)
            matrix(i, j) -= mean(j);

    // Compute covariance matrix
    TMatrixD transposedMatrix = matrix;
    transposedMatrix.T(); // Transpose matrix using a built in function
    TMatrixD covMatrixTemp = (1.0 / (n - 1)) * (transposedMatrix * matrix);
     // Ensure covarianceMatrix is symmetric by copying the upper triangle of covMatrixTemp
     int size = covMatrixTemp.GetNrows();
    //  covarianceMatrix.ResizeTo(size);  // Resize to match covMatrixTemp dimensions
     
     // Copy the upper triangular part of covMatrixTemp into covarianceMatrix
     for (int i = 0; i < size; ++i) {
         for (int j = i; j < size; ++j) {
             covarianceMatrix(i, j) = covMatrixTemp(i, j);  // This ensures symmetry
             covarianceMatrix(j, i) = covMatrixTemp(i, j);  // Ensure symmetric
         }
     }
    std::cout << "Covariance Matrix:" << std::endl;
    covarianceMatrix.Print();
    // std::cout << "Covariance Matrix Dimensions: " 
    //       << covarianceMatrix.GetNrows() << " x " 
    //       << covarianceMatrix.GetNcols() << std::endl;
}


// Compute eigenvalues and eigenvectors
void PCA::ComputeEigenDecomposition() {
    ComputeCovarianceMatrix();  // Ensure covariance matrix is computed
    TMatrixDSymEigen eig(covarianceMatrix);
    covarianceMatrix.Print();
    eigenValues.ResizeTo(covarianceMatrix.GetNrows());
    eigenValues = eig.GetEigenValues(); // gets eigenvalues from built in function of root
    std::cout << "EigenValues:" << std::endl;
    eigenValues.Print();
    eigenVectors.ResizeTo(covarianceMatrix.GetNrows(), covarianceMatrix.GetNrows());
    std::cout << "EigenVectors:" << std::endl;
    eigenVectors = eig.GetEigenVectors();
    eigenVectors.Print();
}

// Get sigmas (square root of eigenvalues)
std::vector<double> PCA::GetSigmas() const {
    std::vector<double> sigmas(numFeatures+1);
    // sigmas = {sigmax, sigmay, sigmaxy}
    sigmas = {covarianceMatrix[0][0],covarianceMatrix[1][1],covarianceMatrix[0][1]};
    // Printing elements of vector
    for (double i : sigmas){
        std::cout << i << " ";
        
    }
    std::cout << std::endl;
    return sigmas;
}

#include "MAT_Matrix.h"
#include "NumberTheory.h"
#include "Namelist.h"
#include "MatrixCanonicalForm.h"
#include "Temp_PerfectForm.h"



int main(int argc, char* argv[])
{
  try {
    if (argc != 3) {
      std::cerr << "MakeInitial_FileMatrix [FileIn] [FileOut]\n";
      throw TerminalException{1};
    }
    //
    using T=mpq_class;
    using Tint=int;
    //
    std::string FileIn  = argv[1];
    std::string FileOut = argv[2];
    //
    std::ifstream is(FileIn);
    std::ofstream os(FileIn);
    int nbPerfect;
    is >> nbPerfect;
    os << nbPerfect << "\n";
    std::cerr << "nbPerfect=" << nbPerfect << "\n";
    for (int iPerfect=0; iPerfect<nbPerfect; iPerfect++) {
      MyMatrix<Tint> ePerfect_Tint = ReadMatrix<Tint>(is);
      MyMatrix<T>    ePerfect_T = ConvertMatrixUniversal<T,Tint>(ePerfect_Tint);
      //
      int eStatus=0;
      //
      Tshortest<T,Tint> eRec = T_ShortestVector<T,Tint>(ePerfect_T);
      int incd = (eRec.SHV.rows()) / 2;
      //
      MyMatrix<T>    eMatCan_T = ComputeCanonicalForm<T,Tint>(ePerfect_T).second;
      MyMatrix<Tint> eMatCan_Tint = ConvertMatrixUniversal<Tint,T>(eMatCan_T);
      //
      os << eStatus << "\n";
      os << incd << "\n";
      WriteMatrix(os, eMatCan_Tint);
    }
  }
  catch (TerminalException const& e) {
    exit(e.eVal);
  }
}
#ifndef TEMP_POLYTOPE_EQUI_STAB
#define TEMP_POLYTOPE_EQUI_STAB

#include "GRAPH_bliss.h"
#include "MAT_Matrix.h"
#include "Basic_string.h"
#include "Basic_file.h"
#include "GRAPH_GraphicalFunctions.h"
#include "GRP_GroupFct.h"
#include "COMB_Combinatorics_elem.h"



template<typename T>
T VectorDistance(std::vector<T> const& V1, std::vector<T> const& V2)
{
  int siz=V1.size();
  T MaxNorm=0;
  for (int i=0; i<siz; i++) {
    T eDiff=V1[i] - V2[i];
    T eNorm=T_abs(eDiff);
    if (eNorm > MaxNorm)
      MaxNorm=eNorm;
  }
  return MaxNorm;
}



template<typename T>
int WeighMatrix_IsNear(T eVal1, T eVal2, T eVal3)
{
  T eDiff=eVal1 - eVal2;
  T eAbsDiff=T_abs(eDiff);
  if (eAbsDiff <= eVal3)
    return 1;
  return 0;
}



template<typename T>
int WeighMatrix_IsNear(std::vector<T> V1, std::vector<T> V2, T eVal3)
{
  int siz1=V1.size();
  int siz2=V2.size();
  if (siz1 != siz2)
    return 0;
  T MaxNorm=VectorDistance(V1, V2);
  if (MaxNorm <= eVal3)
    return 1;
  return 0;
}



template<typename T1, typename T2>
struct WeightMatrix {
public:
  // no accidental construction, i.e. temporaries and the like
  WeightMatrix()
  {
    TheTol=-1;
    nbRow=-1;
  }
  WeightMatrix(int const& inpNbRow, T2 const& inpTheTol)
  {
    TheTol=inpTheTol;
    nbRow=inpNbRow;
    int nb=nbRow*nbRow;
    TheMat.resize(nb);
  }
  WeightMatrix(int const& INP_nbRow, std::vector<int> const& INP_TheMat, std::vector<T1> const& INP_ListWeight, T2 const& INP_TheTol) : nbRow(INP_nbRow), TheMat(INP_TheMat), ListWeight(INP_ListWeight), TheTol(INP_TheTol)
  {
  }
  WeightMatrix(WeightMatrix<T1, T2> const& eMat)
  {
    nbRow=eMat.rows();
    TheTol=eMat.GetTol();
    ListWeight=eMat.GetWeight();
    int nb=nbRow*nbRow;
    TheMat.resize(nb);
    for (int iRow=0; iRow<nbRow; iRow++)
      for (int iCol=0; iCol<nbRow; iCol++) {
	int eValue=eMat.GetValue(iRow, iCol);
	int idx=iRow + nbRow*iCol;
	TheMat[idx]=eValue;
      }
  }
  WeightMatrix<T1, T2> operator=(WeightMatrix<T1, T2> const& eMat)
  {
    nbRow=eMat.rows();
    TheTol=eMat.GetTol();
    ListWeight=eMat.GetWeight();
    int nb=nbRow*nbRow;
    TheMat.resize(nb);
    for (int iRow=0; iRow<nbRow; iRow++)
      for (int iCol=0; iCol<nbRow; iCol++) {
	int eValue=eMat.GetValue(iRow, iCol);
	int idx=iRow + nbRow*iCol;
	TheMat[idx]=eValue;
      }
    return *this;
  }
  ~WeightMatrix()
  {
  }
  // Below is lighter stuff
  bool IsSymmetric() const
  {
    for (int iRow=0; iRow<nbRow; iRow++)
      for (int iCol=0; iCol<nbRow; iCol++) {
	int eVal1=GetValue(iRow, iCol);
	int eVal2=GetValue(iCol, iRow);
	if (eVal1 != eVal2)
	  return false;
      }
    return true;
  }
  int rows(void) const
  {
    return nbRow;
  }
  T2 GetTol(void) const
  {
    return TheTol;
  }
  int GetWeightSize(void) const
  {
    int siz=ListWeight.size();
    return siz;
  }
  void Update(int const& iRow, int const& iCol, T1 const& eVal)
  {
    int ThePos, idxMat;
    int WeFound=0;
    int nbEnt=ListWeight.size();
    ThePos=nbEnt;
    for (int i=0; i<nbEnt; i++)
      if (WeFound == 0)
	if (WeighMatrix_IsNear(eVal, ListWeight[i], TheTol) == 1) {
	  WeFound=1;
	  ThePos=i;
	}
    if (WeFound == 0)
      ListWeight.push_back(eVal);
    idxMat=iRow + nbRow*iCol;
    TheMat[idxMat]=ThePos;
  }
  int GetValue(int const& iRow, int const& iCol) const
  {
    int idx=iRow + nbRow*iCol;
    //    std::cerr << "GetValue iRow=" << iRow << " iCol=" << iCol << " idx=" << idx << " value=" << TheMat[idx] << "\n";
    return TheMat[idx];
  }
  void intDirectAssign(int const& iRow, int const& iCol, int const& pos)
  {
    int idx=iRow + nbRow*iCol;
    //    std::cerr << "intDirectAssign iRow=" << iRow << " iCol=" << iCol << " idx=" << idx << " pos=" << pos << "\n";
    TheMat[idx]=pos;
    //    std::cerr << "intDirectAssign TheMat[idx]=" << TheMat[idx] << "\n";
  }
  void SetWeight(std::vector<T1> const & inpWeight)
  {
    ListWeight=inpWeight;
  }
  std::vector<T1> GetWeight() const
  {
    return ListWeight;
  }
  void ReorderingOfWeights(std::vector<int> const& gListRev)
  {
    int nbEnt=ListWeight.size();
#ifdef DEBUG
    int siz=gListRev.size();
    if (nbEnt != siz) {
      std::cerr << "We should have nbEnt = siz\n";
      std::cerr << "nbEnt=" << nbEnt << "\n";
      std::cerr << "siz=" << siz << "\n";
      throw TerminalException{1};
    }
#endif
    for (int iRow=0; iRow<nbRow; iRow++)
      for (int iCol=0; iCol<nbRow; iCol++) {
	int idx=iRow + nbRow*iCol;
	int eValue=TheMat[idx];
	int nValue=gListRev[eValue];
	TheMat[idx]=nValue;
      }
    std::vector<T1> NewListWeight(nbEnt);
    for (int iEnt=0; iEnt<nbEnt; iEnt++) {
      int nEnt=gListRev[iEnt];
      NewListWeight[nEnt]=ListWeight[iEnt];
    }
    ListWeight=NewListWeight;
  }
private:
  int nbRow;
  std::vector<int> TheMat;
  std::vector<T1> ListWeight;
  T2 TheTol;
};


template<typename T1, typename T2>
void ReorderingSetWeight(WeightMatrix<T1,T2> & WMat)
{
  std::vector<T1> ListWeight=WMat.GetWeight();
  std::set<T1> SetWeight;
  for (auto & eVal : ListWeight)
    SetWeight.insert(eVal);
  int nbEnt=ListWeight.size();
  std::vector<int> g(nbEnt);
  for (int iEnt=0; iEnt<nbEnt; iEnt++) {
    T1 eVal = ListWeight[iEnt];
    typename std::set<T1>::iterator it = SetWeight.find(eVal);
    int pos = std::distance(SetWeight.begin(), it);
    g[iEnt] = pos;
  }
  WMat.ReorderingOfWeights(g);
#ifdef DEBUG
  std::vector<T1> ListWeightB=WMat.GetWeight();
  for (int iEnt=1; iEnt<nbEnt; iEnt++) {
    if (ListWeightB[iEnt-1] >= ListWeightB[iEnt]) {
      std::cerr << "ERROR: The ListWeightB is not increasing at iEnt=" << iEnt << "\n";
      throw TerminalException{1};
    }
  }
#endif
}



template<typename T1, typename T2>
bool RenormalizeWeightMatrix(WeightMatrix<T1, T2> const& WMatRef, WeightMatrix<T1, T2> &WMat2)
{
  int nbRow=WMatRef.rows();
  int nbRow2=WMat2.rows();
  if (nbRow != nbRow2)
    return false;
  int nbEnt=WMatRef.GetWeightSize();
  int nbEnt2=WMat2.GetWeightSize();
  if (nbEnt != nbEnt2)
    return false;
  std::vector<T1> ListWeightRef=WMatRef.GetWeight();
  std::vector<T1> ListWeight=WMat2.GetWeight();
  std::vector<int> gListRev(nbEnt);
  T2 TheTol=WMatRef.GetTol();
  for (int i=0; i<nbEnt; i++) {
    int jFound=-1;
    for (int j=0; j<nbEnt; j++)
      if (WeighMatrix_IsNear(ListWeightRef[i], ListWeight[j], TheTol) == 1)
	jFound=j;
    if (jFound == -1)
      return false;
    gListRev[jFound]=i;
  }
  WMat2.ReorderingOfWeights(gListRev);
#ifdef DEBUG
  std::vector<T1> ListWeight1=WMatRef.GetWeight();
  std::vector<T1> ListWeight2=WMat2.GetWeight();
  for (int iEnt=0; iEnt<nbEnt; iEnt++) {
    if (WeighMatrix_IsNear(ListWeight1[iEnt], ListWeight2[iEnt], TheTol) == 1) {
      std::cerr << "ERROR: The reordering failed\n";
      throw TerminalException{1};
    }
  }
#endif
  return true;
}



template<typename T1, typename T2>
struct WeightMatrixFCT {
public:
  // no accidental construction, i.e. temporaries and the like
  WeightMatrixFCT() = delete;
  WeightMatrixFCT(int inpNbRow, T2 inpTheTol, std::function<T1(int,int)> const& eFCT)
  {
    TheTol=inpTheTol;
    nbRow=inpNbRow;
    FCT=eFCT;
    for (int iRow=0; iRow<nbRow; iRow++)
      for (int iCol=0; iCol<nbRow; iCol++) {
	T1 eVal=FCT(iRow,iCol);
	int pos=GetPositionWeight(eVal);
	if (pos == -1)
	  ListWeight.push_back(eVal);
      }
  }
  WeightMatrixFCT(WeightMatrixFCT<T1, T2> const& eMat)
  {
    nbRow=eMat.rows();
    TheTol=eMat.GetTol();
    ListWeight=eMat.GetWeight();
    FCT=eMat.GetFCT();
  }
  WeightMatrixFCT<T1, T2>& operator=(WeightMatrixFCT<T1, T2> const& eMat)
  {
    nbRow=eMat.rows();
    TheTol=eMat.GetTol();
    ListWeight=eMat.GetWeight();
    FCT=eMat.GetFCT();
    return *this;
  }
  ~WeightMatrixFCT() = default;
  // Below is lighter stuff
  bool IsSymmetric() const
  {
    for (int iRow=0; iRow<nbRow; iRow++)
      for (int iCol=0; iCol<nbRow; iCol++) {
	int eVal1=GetValue(iRow, iCol);
	int eVal2=GetValue(iCol, iRow);
	if (eVal1 != eVal2)
	  return false;
      }
    return true;
  }
  int rows(void) const
  {
    return nbRow;
  }
  T2 GetTol(void) const
  {
    return TheTol;
  }
  int GetWeightSize(void) const
  {
    int siz=ListWeight.size();
    return siz;
  }
  int GetPositionWeight(T1 eVal) const
  {
    int nbEnt=ListWeight.size();
    for (int i=0; i<nbEnt; i++)
      if (WeighMatrix_IsNear(eVal, ListWeight[i], TheTol) == 1)
	return i;
    return -1;
  }
  int GetValue(int const& iRow, int const& iCol) const
  {
    T1 eVal=FCT(iRow,iCol);
    int pos=GetPositionWeight(eVal);
#ifdef DEBUG
    if (pos == -1) {
      std::cerr << "We should not reach that stage\n";
      throw TerminalException{1};
    }
#endif
    return pos;
  }
  void SetWeight(std::vector<T1> const & inpWeight)
  {
    ListWeight=inpWeight;
  }
  std::vector<T1> GetWeight() const
  {
    return ListWeight;
  }
  std::function<T1(int,int)> GetFCT() const
  {
    return FCT;
  }
  void ReorderingOfWeights(std::vector<int> const& gListRev)
  {
    int nbEnt=ListWeight.size();
    std::vector<T1> NewListWeight(nbEnt);
    for (int iEnt=0; iEnt<nbEnt; iEnt++) {
      int nEnt=gListRev[iEnt];
      NewListWeight[nEnt]=ListWeight[iEnt];
    }
    ListWeight=NewListWeight;
  }
private:
  int nbRow;
  std::function<T1(int,int)> FCT;
  std::vector<T1> ListWeight;
  T2 TheTol;
};



template<typename T>
WeightMatrix<T,T> T_TranslateToMatrix(MyMatrix<T> const& eMat, T const & TheTol)
{
  int nbRow=eMat.rows();
  WeightMatrix<T,T> WMat=WeightMatrix<T,T>(nbRow, TheTol);
  for (int iRow=0; iRow<nbRow; iRow++)
    for (int iCol=0; iCol<nbRow; iCol++) {
      T eVal=eMat(iRow, iCol);
      WMat.Update(iCol, iRow, eVal);
    }
  return WMat;
}



template<typename T, typename Tint>
WeightMatrix<T,T> T_TranslateToMatrix_QM_SHV(MyMatrix<T> const& qMat, MyMatrix<Tint> const& SHV, T const & TheTol)
{
  int nbRow=SHV.rows();
  int n=qMat.rows();
  WeightMatrix<T,T> WMat=WeightMatrix<T,T>(nbRow, TheTol);
  int nbPair=nbRow / 2;
  for (int iPair=0; iPair<nbPair; iPair++) {
    MyVector<T> V(n);
    for (int i=0; i<n; i++) {
      T eVal=0;
      for (int j=0; j<n; j++)
	eVal += qMat(j,i) * SHV(2*iPair, j);
      V(i) = eVal;
    }
    for (int jPair=iPair; jPair<nbPair; jPair++) {
      T eScal=0;
      for (int i=0; i<n; i++)
	eScal += V(i)*SHV(2*jPair,i);
      WMat.Update(2*iPair  , 2*jPair  , eScal);
      WMat.Update(2*iPair+1, 2*jPair  , -eScal);
      WMat.Update(2*iPair  , 2*jPair+1, -eScal);
      WMat.Update(2*iPair+1, 2*jPair+1, eScal);
      if (iPair != jPair) {
	WMat.Update(2*jPair  , 2*iPair  , eScal);
	WMat.Update(2*jPair+1, 2*iPair  , -eScal);
	WMat.Update(2*jPair  , 2*iPair+1, -eScal);
	WMat.Update(2*jPair+1, 2*iPair+1, eScal);
      }
    }
  }
  return WMat;
}







template<typename T>
WeightMatrix<T,T> T_TranslateToMatrixOrder(MyMatrix<T> const& eMat)
{
  int nbRow=eMat.rows();
  std::set<T> SetScal;
  for (int iRow=0; iRow<nbRow; iRow++)
    for (int iCol=0; iCol<nbRow; iCol++) {
      T eVal=eMat(iRow, iCol);
      SetScal.insert(eVal);
    }
  std::vector<T> VectScal;
  for (auto & eVal : SetScal)
    VectScal.push_back(eVal);
  std::vector<int> TheMat(nbRow*nbRow);
  int idx=0;
  for (int iCol=0; iCol<nbRow; iCol++) {
    for (int iRow=0; iRow<nbRow; iRow++) {
      T eVal=eMat(iRow,iCol);
      typename std::set<T>::iterator it = SetScal.find(eVal);
      int pos = std::distance(SetScal.begin(), it);
      TheMat[idx] = pos;
      idx++;
    }
  }
  T TheTol=0;
  return WeightMatrix<T,T>(nbRow, TheMat, VectScal, TheTol);
}






bliss::Graph* ReadGraphFromFile(FILE *f, unsigned int &nof_vertices)
{
  unsigned int nof_edges;
  int ret;
  bliss::Graph *g =0;
  ret=fscanf(f, "%u %u\n", &nof_vertices, &nof_edges);
  if (ret != 1) {
    std::cerr << "fscanf error while reading graph 1\n";
    throw TerminalException{1};
  }
  g = new bliss::Graph(nof_vertices);
  for (int i=0; i<int(nof_vertices); i++) {
    unsigned int color;
    ret=fscanf(f, "%u\n", &color);
    if (ret != 1) {
      std::cerr << "fscanf error while reading graph 2\n";
      throw TerminalException{1};
    }
    g->change_color(i, color);
  }
  for (int iEdge=0; iEdge<int(nof_edges); iEdge++) {
    int a, b;
    ret=fscanf(f, "%u %u\n", &a, &b);
    if (ret != 1) {
      std::cerr << "fscanf error while reading graph 3\n";
      throw TerminalException{1};
    }
    g->add_edge(a-1, b-1);
  }
  return g;
}



template<typename T>
MyMatrix<T> GetQmatrix(MyMatrix<T> const& TheEXT)
{
  int nbRow=TheEXT.rows();
  int nbCol=TheEXT.cols();
  MyMatrix<T> QMat(nbCol, nbCol);
  for (int iCol=0; iCol<nbCol; iCol++)
    for (int jCol=0; jCol<nbCol; jCol++) {
      T eSum=0;
      for (int iRow=0; iRow<nbRow; iRow++)
	eSum += TheEXT(iRow, jCol) * TheEXT(iRow, iCol);
      QMat(iCol, jCol)=eSum;
    }
  return Inverse(QMat);
}



template<typename T>
WeightMatrix<T, T> GetSimpleWeightMatrix(MyMatrix<T> const& TheEXT, MyMatrix<T> const& Qmat)
{
  int nbRow=TheEXT.rows();
  int nbCol=TheEXT.cols();
  T TheTol=0;
  WeightMatrix<T,T> WMat=WeightMatrix<T,T>(nbRow, TheTol);
  for (int iRow=0; iRow<nbRow; iRow++)
    for (int jRow=0; jRow<nbRow; jRow++) {
      T eSum=0;
      for (int iCol=0; iCol<nbCol; iCol++)
	for (int jCol=0; jCol<nbCol; jCol++)
	  eSum += Qmat(iCol, jCol) * TheEXT(iRow, iCol) * TheEXT(jRow, jCol);
      WMat.Update(iRow, jRow, eSum);
    }
  return WMat;
}



template<typename T>
WeightMatrix<T, T> GetWeightMatrix(MyMatrix<T> const& TheEXT)
{
  MyMatrix<T> Qmat=GetQmatrix(TheEXT);
  return GetSimpleWeightMatrix(TheEXT, Qmat);
}



template<typename T>
WeightMatrix<std::vector<T>, T> GetWeightMatrix_ListComm(MyMatrix<T> const& TheEXT, MyMatrix<T> const&GramMat, std::vector<MyMatrix<T> > const& ListComm)
{
  int nbRow=TheEXT.rows();
  int nbCol=TheEXT.cols();
  int nbComm=ListComm.size();
  T TheTol=0;
  WeightMatrix<std::vector<T>, T> WMat=WeightMatrix<std::vector<T>, T>(nbRow, TheTol);
  std::vector<MyMatrix<T> > ListProd;
  ListProd.push_back(GramMat);
  for (int iComm=0; iComm<nbComm; iComm++) {
    MyMatrix<T> eProd=ListComm[iComm]*GramMat;
    ListProd.push_back(eProd);
  }
  for (int iRow=0; iRow<nbRow; iRow++)
    for (int jRow=0; jRow<nbRow; jRow++) {
      T eZer=0;
      std::vector<T> eVectSum(nbComm+1,eZer);
      for (int iCol=0; iCol<nbCol; iCol++)
	for (int jCol=0; jCol<nbCol; jCol++) {
	  T eProd=TheEXT(iRow, iCol) * TheEXT(jRow, jCol);
	  for (int iMat=0; iMat<=nbComm; iMat++)
	    eVectSum[iMat] += eProd * ListProd[iMat](iCol, jCol);
	}
      WMat.Update(iRow, jRow, eVectSum);
    }
  return WMat;
}



template<typename T>
WeightMatrix<std::vector<T>, T> GetWeightMatrix_ListMatrix(std::vector<MyMatrix<T> > const& ListMatrix, MyMatrix<T> const& TheEXT)
{
  int nbRow=TheEXT.rows();
  int nbCol=TheEXT.cols();
  int nbMat=ListMatrix.size();
  T TheTol=0;
  WeightMatrix<std::vector<T>, T> WMat(nbRow, TheTol);
  for (int iRow=0; iRow<nbRow; iRow++)
    for (int jRow=0; jRow<nbRow; jRow++) {
      std::vector<T> eVectScal(nbMat,0);
      for (int iCol=0; iCol<nbCol; iCol++)
	for (int jCol=0; jCol<nbCol; jCol++) {
	  T eProd=TheEXT(iRow, iCol) * TheEXT(jRow, jCol);
	  for (int iMat=0; iMat<nbMat; iMat++)
	    eVectScal[iMat] += eProd * ListMatrix[iMat](iCol, jCol);
	}
      WMat.Update(iRow, jRow, eVectScal);
    }
  return WMat;
}



template<typename T>
WeightMatrix<T, T> GetWeightMatrixGramMatShort(MyMatrix<T> const& TheGramMat, MyMatrix<int> const& ListShort, T const& TheTol)
{
  int nbShort=ListShort.rows();
  int n=TheGramMat.rows();
  WeightMatrix<T,T> WMat=WeightMatrix<T,T>(nbShort, TheTol);
  for (int iShort=0; iShort<nbShort; iShort++)
    for (int jShort=0; jShort<nbShort; jShort++) {
      T eScal=0;
      for (int i=0; i<n; i++)
	for (int j=0; j<n; j++) {
	  int eVal12=ListShort(iShort, i) * ListShort(jShort, j);
	  eScal += eVal12 * TheGramMat(i, j);
	}
      WMat.Update(iShort, jShort, eScal);
    }
  std::cerr << "|ListWeight|=" << WMat.GetWeightSize() << "\n";
  return WMat;
}



template<typename T>
WeightMatrix<T, T> GetWeightMatrixGramMatShort_Fast(MyMatrix<T> const& TheGramMat, MyMatrix<int> const& ListShort)
{
  int nbShort=ListShort.rows();
  //  std::cerr << "nbShort=" << nbShort << "\n";
  int n=TheGramMat.rows();
  auto GetValue=[&](int const&iShort, int const&jShort) -> T {
    T eScal=0;
    for (int i=0; i<n; i++)
      for (int j=0; j<n; j++) {
	int eVal12=ListShort(iShort, i) * ListShort(jShort, j);
	eScal += eVal12 * TheGramMat(i,j);
      }
    return eScal;
  };
  MyMatrix<T> ScalMat(nbShort,nbShort);
  std::set<T> setWeight;
  for (int iShort=0; iShort<nbShort; iShort++)
    for (int jShort=0; jShort<=iShort; jShort++) {
      T eScal=GetValue(iShort,jShort);
      ScalMat(iShort,jShort)=eScal;
      ScalMat(jShort,iShort)=eScal;
      setWeight.insert(eScal);
    }
  //  std::cerr << "|setWeight|=" << setWeight.size() << "\n";
  struct PairData {
    T x;
    size_t idx;
  };
  auto comp=[&](PairData const& a, PairData const& b) -> bool {
    if (a.x < b.x)
      return true;
    if (a.x > b.x)
      return false;
    return false;
  };
  std::set<PairData,decltype(comp)> setWeightIdx(comp);
  std::vector<T> INP_ListWeight;
  size_t idx=0;
  for (auto & eX : setWeight) {
    setWeightIdx.insert({eX,idx});
    INP_ListWeight.push_back(eX);
    idx++;
  }
  std::vector<int> INP_TheMat(nbShort*nbShort);
  for (int iShort=0; iShort<nbShort; iShort++)
    for (int jShort=0; jShort<=iShort; jShort++) {
      T eScal=GetValue(iShort,jShort);
      PairData test{eScal,0};
      auto iter=setWeightIdx.find(test);
#ifdef DEBUG
      if (iter == setWeightIdx.end()) {
	std::cerr << "Without a doubt a bug\n";
	throw TerminalException{1};
      }
#endif
      int idxret=iter->idx;
      //      std::cerr << "idx=" << idx << "\n";
      int pos1=iShort + nbShort*jShort;
      int pos2=jShort + nbShort*iShort;
      INP_TheMat[pos1]=idxret;
      INP_TheMat[pos2]=idxret;
    }
  T INP_TheTol=0;
  WeightMatrix<T,T> WMat(nbShort, INP_TheMat, INP_ListWeight, INP_TheTol);
  return WMat;
}



template<typename T>
void GetSymmGenerateValue(T const& rVal, T & eRet)
{
  eRet=rVal;
}



template<typename T>
void GetSymmGenerateValue(T const& rVal, std::vector<T> & eVect)
{
  eVect.push_back(rVal);
}



template<typename T1, typename T2>
WeightMatrix<T1, T2> GetSymmetricWeightMatrix(WeightMatrix<T1,T2> const& WMatI)
{
  std::set<T1> setWeight;
  std::vector<T1> ListWeight;
  int nbRow=WMatI.rows();
  T2 TheTol=WMatI.GetTol();
  WeightMatrix<T1,T2> WMatO=WeightMatrix<T1,T2>(2*nbRow, TheTol);
  int siz=WMatI.GetWeightSize();
  for (int iRow=0; iRow<nbRow; iRow++)
    for (int jRow=0; jRow<nbRow; jRow++) {
      int pos=WMatI.GetValue(iRow, jRow);
      WMatO.intDirectAssign(iRow, jRow+nbRow, pos);
      WMatO.intDirectAssign(jRow+nbRow, iRow, pos);
    }
  for (int iRow=0; iRow<nbRow; iRow++)
    for (int jRow=0; jRow<nbRow; jRow++) {
      WMatO.intDirectAssign(iRow      , jRow      , siz);
      WMatO.intDirectAssign(iRow+nbRow, jRow+nbRow, siz+1);
    }
  for (auto& eWei : WMatI.GetWeight())
    setWeight.insert(eWei);
  ListWeight=WMatI.GetWeight();
  int iVal=1;
  for (int j=0; j<2; j++) {
    while(true) {
      T1 genVal;
      T2 eVal;
      eVal=iVal;
      GetSymmGenerateValue<T2>(eVal, genVal);
      typename std::set<T1>::iterator iterTEST=setWeight.find(genVal);
      if (iterTEST == setWeight.end()) {
	setWeight.insert(genVal);
	ListWeight.push_back(genVal);
	break;
      }
      iVal++;
    }
  }
  WMatO.SetWeight(ListWeight);
  return WMatO;
}



template<typename T1, typename T2>
TheGroupFormat GetStabilizerAsymmetricMatrix(WeightMatrix<T1,T2> const& WMatI)
{
  WeightMatrix<T1, T2> WMatO=GetSymmetricWeightMatrix(WMatI);
  int nbSHV=WMatI.rows();
  TheGroupFormat GRP=GetStabilizerWeightMatrix(WMatO);
  std::vector<permlib::dom_int> v(nbSHV);
  std::vector<permlib::Permutation> ListGen;
  for (auto & eGen : GRP.group->S) {
    for (int iSHV=0; iSHV<nbSHV; iSHV++) {
      int jSHV=eGen->at(iSHV);
      v[iSHV]=jSHV;
    }
    ListGen.push_back(permlib::Permutation(v));
  }
  return GetPermutationGroup(nbSHV, ListGen);
}



template<typename T1, typename T2>
EquivTest<permlib::Permutation> GetEquivalenceAsymmetricMatrix(WeightMatrix<T1,T2> const& WMat1, WeightMatrix<T1,T2> const& WMat2)
{
  WeightMatrix<T1, T2> WMatO1=GetSymmetricWeightMatrix<T1, T2>(WMat1);
  WeightMatrix<T1, T2> WMatO2=GetSymmetricWeightMatrix<T1, T2>(WMat2);
  EquivTest<permlib::Permutation> eResEquiv=TestEquivalenceWeightMatrix(WMatO1, WMatO2);
  if (!eResEquiv.TheReply)
    return eResEquiv;
  int nbSHV=WMat1.rows();
  std::vector<permlib::dom_int> v(nbSHV);
  for (int i=0; i<nbSHV; i++)
    v[i]=eResEquiv.TheEquiv.at(i);
  return {true, permlib::Permutation(v)};
}



static void report_aut_vectvectint(void* param, const unsigned int n, const unsigned int* aut)
{
  std::vector<int> eVect;
  for (int i=0; i<(int)n; i++)
    eVect.push_back(aut[i]);
  ((VectVectInt*)param)->push_back(eVect);
}





TheGroupFormat GetGraphAutomorphismGroup(bliss::Graph *g, unsigned int nof_vertices)
{
  std::vector<permlib::Permutation::ptr> generatorList;
  bliss::Stats stats;
  VectVectInt ListGen;
  VectVectInt *h;
  h=&ListGen;
  g->find_automorphisms(stats, &report_aut_vectvectint, (void *)h);
  delete g;
  int nbGen=ListGen.size();
  for (int iGen=0; iGen<nbGen; iGen++) {
    std::vector<permlib::dom_int> gList(nof_vertices);
    for (int i=0; i<(int)nof_vertices; i++)
      gList[i]=ListGen[iGen][i];
    generatorList.push_back(permlib::Permutation::ptr(new permlib::Permutation(gList)));
  }
  TheGroupFormat GRP;
  GRP.n=nof_vertices;
  GRP.group=construct(nof_vertices, generatorList.begin(), generatorList.end());
  GRP.size=GRP.group->order<mpz_class>();
  return GRP;
}



int GetNeededPower(int nb)
{
  int h=0;
  int eExpo=1;
  while(true) {
    if (nb < eExpo)
      return h;
    h++;
    eExpo=eExpo*2;
  }
}



std::vector<int> GetBinaryExpression(int eVal, int h)
{
  int eWork, eExpo, eExpoB, i, res;
  eWork=eVal;
  eExpo=1;
  std::vector<int> eVect(h);
  for (i=0; i<h; i++) {
    eExpoB=eExpo*2;
    res=eWork % eExpoB;
    eVect[i]=res/eExpo;
    eExpo=eExpoB;
    eWork=eWork - res;
  }
  return eVect;
}



template<typename T1, typename T2, typename Tout>
std::vector<Tout> GetLocalInvariantWeightMatrix(WeightMatrix<T1,T2> const&WMat, Face const& eSet)
{
  int nbVert=eSet.count();
  std::vector<int> eList(nbVert);
  int aRow=eSet.find_first();
  for (int i=0; i<nbVert; i++) {
    eList[i]=aRow;
    aRow=eSet.find_next(aRow);
  }
  int nbWeight=WMat.GetWeightSize();
  std::vector<Tout> eInv(nbWeight,0);
  for (int iVert=0; iVert<nbVert; iVert++) {
    int aVert=eList[iVert];
    for (int jVert=0; jVert<nbVert; jVert++) {
      int bVert=eList[jVert];
      int iWeight=WMat.GetValue(aVert, bVert);
      eInv[iWeight]++;
    }
  }
  return eInv;
}



template<typename T1, typename T2>
WeightMatrix<T1,T2> WeightMatrixFromPairOrbits(TheGroupFormat const& GRP, std::ostream & os)
{
  bool IsDiag;
  int n=GRP.n;
  WeightMatrix<T1,T2> WMat(n,0);
  for (int i=0; i<n; i++)
    for (int j=0; j<n; j++)
      WMat.intDirectAssign(i,j,-1);
  auto GetUnset=[&]() -> std::pair<int,int> {
    for (int i=0; i<n; i++)
      for (int j=0; j<n; j++) {
	int eVal=WMat.GetValue(i,j);
	if (eVal == -1) {
	  return {i,j};
	}
      }
    return {-1,-1};
  };
  struct VectorListPair {
    std::vector<std::pair<int,int>> ListWorkingPair;
    int nbWorkingPair=0;
  };
  VectorListPair VLP0, VLP1;
  auto FuncInsert=[&](VectorListPair & VLP, std::pair<int,int> const& ePair) -> void {
    if (VLP.nbWorkingPair < int(VLP.ListWorkingPair.size())) {
      VLP.ListWorkingPair[VLP.nbWorkingPair]=ePair;
    }
    else {
      VLP.ListWorkingPair.push_back(ePair);
    }
    VLP.nbWorkingPair++;
    int i=ePair.first;
    int j=ePair.second;
    WMat.intDirectAssign(i,j,-2);
  };
  auto FuncInsertIChoice=[&](int const& iChoice, std::pair<int,int> const& ePair) -> void {
    if (iChoice == 0)
      FuncInsert(VLP0, ePair);
    else
      FuncInsert(VLP1, ePair);
  };
  auto SetZero=[&](int const& iChoice) -> void {
    if (iChoice == 0)
      VLP0.nbWorkingPair=0;
    else
      VLP1.nbWorkingPair=0;
  };
  auto GetNbWorkingPair=[&](int const& iChoice) -> int {
    if (iChoice == 0)
      return VLP0.nbWorkingPair;
    else
      return VLP1.nbWorkingPair;
  };
  auto GetEntry=[&](int const& iChoice, int const& iPair) -> std::pair<int,int> {
    if (iChoice == 0)
      return VLP0.ListWorkingPair[iPair];
    else
      return VLP1.ListWorkingPair[iPair];
  };
  int iChoice=0;
  int iOrbit=0;
  std::vector<T1> ListWeight;
  //  bool DoDebug=true;
  bool DoDebug=false;
  while(true) {
    std::pair<int,int> eStart=GetUnset();
    if (eStart.first == -1)
      break;
    IsDiag=false;
    if (eStart.first == eStart.second)
      IsDiag=true;
    if (DoDebug) {
      os << "iOrbit=" << iOrbit << " eStart=" << eStart.first << " , " << eStart.second << "\n";
      std::cerr << "iOrbit=" << iOrbit << " eStart=" << eStart.first << " , " << eStart.second << "\n";
      std::cerr << "  IsDiag=" << IsDiag << "\n";
    }
    T1 insVal=iOrbit;
    ListWeight.push_back(insVal);
    FuncInsertIChoice(iChoice, eStart);
    int orbSize=0;
    while(true) {
      int iChoiceB=1-iChoice;
      int nbPair=GetNbWorkingPair(iChoice);
      orbSize += nbPair;
      if (nbPair == 0)
	break;
      for (int iPair=0; iPair<nbPair; iPair++) {
	std::pair<int,int> ePair=GetEntry(iChoice,iPair);
	int i=ePair.first;
	int j=ePair.second;
	WMat.intDirectAssign(i,j,iOrbit);
	for (auto & eGen : GRP.group->S) {
	  int iImg=eGen->at(i);
	  int jImg=eGen->at(j);
	  auto aInsert=[&](int const& u, int const& v) -> void {
	    int eVal1=WMat.GetValue(u,v);
#ifdef DEBUG
	    if (IsDiag) {
	      if (u != v) {
		std::cerr << "IsDiag=" << IsDiag << "\n";
		std::cerr << "  i=" << i << " j=" << j << "\n";
		std::cerr << "  iImg=" << iImg << " jImg=" << jImg << "\n";
		std::cerr << "  Error u=" << u << " v=" << v << "\n";
		throw TerminalException{1};
	      }
	    }
	    else {
	      if (u == v) {
		std::cerr << "IsDiag=" << IsDiag << "\n";
		std::cerr << "  i=" << i << " j=" << j << "\n";
		std::cerr << "  iImg=" << iImg << " jImg=" << jImg << "\n";
		std::cerr << "  Error u=" << u << " v=" << v << "\n";
		throw TerminalException{1};
	      }
	    }
#endif
	    if (eVal1 == -1)
	      FuncInsertIChoice(iChoiceB, {u, v});
	  };
	  aInsert(iImg, jImg);
	  aInsert(jImg, iImg);
	}
      }
      SetZero(iChoice);
      iChoice=iChoiceB;
    }
    if (DoDebug)
      os << "     size=" << GRP.size << " orbSize=" << orbSize << "\n";
    iOrbit++;
  }
  WMat.SetWeight(ListWeight);
  if (DoDebug) {
    for (int i=0; i<n; i++)
      os << "i=" << i << "/" << n << " val=" << WMat.GetValue(i,i) << "\n";
  }
  return WMat;
}



struct LocalInvInfo {
  int nbDiagCoeff;
  int nbOffCoeff;
  std::vector<int> MapDiagCoeff;
  std::vector<int> MapOffCoeff;
  MyMatrix<int> ListChosenTriple;
  WeightMatrix<int,int> WMatInt;
};



template<typename T1, typename T2>
LocalInvInfo ComputeLocalInvariantStrategy(WeightMatrix<T1,T2> const&WMat, TheGroupFormat const& GRP, std::string const& strat, std::ostream & os)
{
  os << "ComputeLocalInvariantStrategy, step 1\n";
  int nbNeed=0;
  bool UsePairOrbit=false;
  std::vector<std::string> LStr=STRING_Split(strat, ":");
  os << "ComputeLocalInvariantStrategy, step 2\n";
  if (LStr[0] != "pairinv") {
    std::cerr << "Now we have only pairinv as simple invariant\n";
    throw TerminalException{1};
  }
  os << "ComputeLocalInvariantStrategy, step 3\n";
  for (int iStr=1; iStr<int(LStr.size()); iStr++) {
    std::string eStr=LStr[iStr];
    std::vector<std::string> LStrB=STRING_Split(eStr, "_");
    if (LStrB[0] == "target") {
      int nbNeed;
      std::istringstream(LStrB[1]) >> nbNeed;
    }
    if (eStr == "use_pair_orbit") {
      UsePairOrbit=true;
    }
  }
  os << "ComputeLocalInvariantStrategy, step 4\n";
  os << "nbNeed=" << nbNeed << "\n";
  os << "UsePairOrbit=" << UsePairOrbit << "\n";
  //
  WeightMatrix<int,int> WMatInt;
  if (UsePairOrbit) {
    WMatInt=WeightMatrixFromPairOrbits<int,int>(GRP, os);
  }
  else {
    WMatInt=NakedWeightedMatrix(WMat);
  }
  os << "ComputeLocalInvariantStrategy, step 5\n";
  //
  int nbRow=WMatInt.rows();
  int nbWeight=WMatInt.GetWeightSize();
  os << "nbRow=" << nbRow << " nbWeight=" << nbWeight << "\n";
  std::vector<int> StatusDiag(nbWeight,0);
  std::vector<int> StatusOff(nbWeight,0);
  os << "ComputeLocalInvariantStrategy, step 5.1\n";
  for (int i=0; i<nbRow; i++) {
    int iWeight=WMatInt.GetValue(i,i);
    //    os << " i=" << i << " iWeight=" << iWeight << "\n";
    StatusDiag[iWeight]=1;
  }
  os << "ComputeLocalInvariantStrategy, step 5.2\n";
  for (int i=0; i<nbRow-1; i++)
    for (int j=i+1; j<nbRow; j++) {
      int iWeight=WMatInt.GetValue(i,j);
      //      os << " i=" << i << " j=" << j << " iWeight=" << iWeight << "\n";
      StatusOff[iWeight]=1;
    }
  os << "ComputeLocalInvariantStrategy, step 6\n";
  int nbDiagCoeff=VectorSum(StatusDiag);
  int nbOffCoeff=VectorSum(StatusOff);
  std::vector<int> MapDiagCoeff(nbWeight,-1);
  std::vector<int> MapOffCoeff(nbWeight,-1);
  int idxA=0;
  for (int i=0; i<nbWeight; i++)
    if (StatusDiag[i] == 1) {
      MapDiagCoeff[i]=idxA;
      idxA++;
    }
  os << "ComputeLocalInvariantStrategy, step 7\n";
  int idxB=0;
  for (int i=0; i<nbWeight; i++)
    if (StatusOff[i] == 1) {
      MapOffCoeff[i]=idxB;
      idxB++;
    }
  os << "ComputeLocalInvariantStrategy, step 8\n";
  LocalInvInfo eInv;
  eInv.nbDiagCoeff=nbDiagCoeff;
  eInv.nbOffCoeff=nbOffCoeff;
  eInv.MapDiagCoeff=MapDiagCoeff;
  eInv.MapOffCoeff=MapOffCoeff;
  eInv.WMatInt=WMatInt;
  //
  int nbNeedTriple=nbNeed - nbDiagCoeff - 2*nbOffCoeff;
  if (nbNeedTriple <= 0)
    return eInv;
  std::set<std::vector<int>> ListEnt;
  std::map<std::vector<int>,int> MapMult;
  auto InsertEntry=[&](std::vector<int> const& rVect) -> void {
    auto iter=ListEnt.find(rVect);
    if (iter == ListEnt.end()) {
      ListEnt.insert(rVect);
      MapMult[rVect]=1;
    }
    else {
      MapMult[rVect]++;
    }
  };
  std::vector<std::vector<int>> ListIndices{{0,1},{0,2},{1,2}};
  std::vector<int> eVect=BinomialStdvect_First(3);
  std::vector<int> colorVect(3);
  while(true) {
    for (int i=0; i<3; i++) {
      int iIdx=ListIndices[i][0];
      int jIdx=ListIndices[i][1];
      int iVert=eVect[iIdx];
      int jVert=eVect[jIdx];
      int iWeight=WMat.GetValue(iVert,jVert);
      colorVect[i]=iWeight;
    }
    std::sort(colorVect.begin(), colorVect.end());
    InsertEntry(colorVect);
    bool test=BinomialStdvect_Increment(nbRow,3,eVect);
    if (!test)
      break;
  }
  struct pairEntInt {
    std::vector<int> eEnt;
    int mult;
  };
  std::function<bool(pairEntInt const&,pairEntInt const&)> secondComp=[&](pairEntInt const& x,pairEntInt const& y) -> bool {
    if (x.mult < y.mult)
      return true;
    if (x.mult > y.mult)
      return false;
    return x.eEnt < y.eEnt;
  };
  std::set<pairEntInt,decltype(secondComp)> ListEntSecond(secondComp);
  for (auto & uVect : ListEnt) {
    int eMult=MapMult[uVect];
    ListEntSecond.insert({uVect,eMult});
  }
  auto iter=ListEntSecond.end();
  MyMatrix<int> ListChosenTriple(nbNeedTriple,3);
  for (int idx=0; idx<nbNeedTriple; idx++) {
    iter--;
    for (int i=0; i<3; i++)
      ListChosenTriple(idx,i) = iter->eEnt[i];
  }
  eInv.ListChosenTriple=ListChosenTriple;
  return eInv;
}



template<typename Tout>
std::vector<Tout> GetLocalInvariantWeightMatrix_Enhanced(LocalInvInfo const& LocalInv, Face const& eSet)
{
  int nbVert=eSet.count();
  int nbRow=LocalInv.WMatInt.rows();
  int nbVertCompl=nbRow - nbVert;
  std::vector<int> eList(nbVert);
  std::vector<int> eListCompl(nbVertCompl);
  int idx1=0;
  int idx2=0;
  for (int i=0; i<nbRow; i++) {
    if (eSet[i] == 1) {
      eList[idx1]=i;
      idx1++;
    }
    else {
      eListCompl[idx2]=i;
      idx2++;
    }
  }
  int nbDiagCoeff=LocalInv.nbDiagCoeff;
  int nbOffCoeff=LocalInv.nbOffCoeff;
  int nbTriple=LocalInv.ListChosenTriple.rows();
  std::vector<Tout> eInv(nbDiagCoeff + 2*nbOffCoeff + nbTriple,0);
  int posShift=0;
  for (int iVert=0; iVert<nbVert; iVert++) {
    int aVert=eList[iVert];
    int iWeight=LocalInv.WMatInt.GetValue(aVert, aVert);
    int iMap=LocalInv.MapDiagCoeff[iWeight];
    eInv[posShift + iMap]++;
  }
  posShift += nbDiagCoeff;
  for (int iVert=0; iVert<nbVert-1; iVert++) {
    int aVert=eList[iVert];
    for (int jVert=iVert+1; jVert<nbVert; jVert++) {
      int bVert=eList[jVert];
      int iWeight=LocalInv.WMatInt.GetValue(aVert, bVert);
      int iMap=LocalInv.MapOffCoeff[iWeight];
      eInv[posShift + iMap]++;
    }
  }
  posShift += nbOffCoeff;
  for (int iVert=0; iVert<nbVert; iVert++) {
    int aVert=eList[iVert];
    for (int jVert=0; jVert<nbVertCompl; jVert++) {
      int bVert=eListCompl[jVert];
      int iWeight=LocalInv.WMatInt.GetValue(aVert,bVert);
      int iMap=LocalInv.MapOffCoeff[iWeight];
      eInv[posShift + iMap]++;
    }
  }
  posShift += nbOffCoeff;
  if (nbTriple == 0)
    return eInv;
  std::vector<int> colorVect(3);
  auto FindITriple=[&]() -> int {
    for (int iTriple=0; iTriple<nbTriple; iTriple++) {
      bool IsMatch=true;
      for (int i=0; i<3; i++)
	if (colorVect[i] != LocalInv.ListChosenTriple(iTriple,i))
	  IsMatch=false;
      if (IsMatch)
	return iTriple;
    }
    return -1;
  };
  std::vector<std::vector<int>> ListIndices{{0,1},{0,2},{1,2}};
  std::vector<int> eVect=BinomialStdvect_First(3);
  while(true) {
    for (int i=0; i<3; i++) {
      int iIdx=ListIndices[i][0];
      int jIdx=ListIndices[i][1];
      int iVert=eVect[iIdx];
      int jVert=eVect[jIdx];
      int aVert=eList[iVert];
      int bVert=eList[jVert];
      int iWeight=LocalInv.WMatInt.GetValue(aVert,bVert);
      colorVect[i]=iWeight;
    }
    std::sort(colorVect.begin(), colorVect.end());
    int iMatchTriple=FindITriple();
    if (iMatchTriple != -1)
      eInv[posShift + iMatchTriple]++;
    bool test=BinomialStdvect_Increment(nbVert,3,eVect);
    if (!test)
      break;
  }
  return eInv;
}



template<typename T>
inline typename std::enable_if<is_totally_ordered<T>::value,T>::type GetInvariantWeightMatrix(WeightMatrix<T,T> const& WMat)
{
  static_assert(is_totally_ordered<T>::value, "Requires T to be totally ordered");
  int nbInv=3;
  int nbVert=WMat.rows();
  std::vector<int> ListM(nbInv);
  ListM[0]=2;
  ListM[1]=5;
  ListM[2]=23;
  int nbWeight=WMat.GetWeightSize();
  std::vector<T> ListWeight=WMat.GetWeight();
  std::vector<int> ListAtt(nbWeight,0);
  for (int iVert=0; iVert<nbVert; iVert++)
    for (int jVert=0; jVert<nbVert; jVert++) {
      int iWeight=WMat.GetValue(iVert, jVert);
      ListAtt[iWeight]++;
    }
  permlib::Permutation ePerm=SortingPerm(ListWeight);
  //  std::cerr << "nbWeight=" << nbWeight << "\n";
#ifdef DEBUG
  for (int jWeight=1; jWeight<nbWeight; jWeight++) {
    int iWeight=jWeight-1;
    int i=ePerm.at(iWeight);
    int j=ePerm.at(jWeight);
    if (ListWeight[i] > ListWeight[j]) {
      std::cerr << "Logical error in the comparison\n";
      throw TerminalException{1};
    }
  }
#endif
  T eMainInv=0;
  for (int iInv=0; iInv<nbInv; iInv++) {
    T eInv=0;
    T ePow=ListM[iInv];
    for (int iWeight=0; iWeight<nbWeight; iWeight++) {
      int jWeight=ePerm.at(iWeight);
      T prov2=ListAtt[jWeight]*ListWeight[jWeight];
      eInv *= ePow;
      eInv += prov2;
    }
    eMainInv += eInv;
  }
  return eMainInv;
}



template<typename T>
WeightMatrix<T,T> ReadWeightedMatrix(std::istream &is)
{
  int nbRow, iRow, jRow, eVal;
  int iEnt, nbEnt;
  T eVal_T;
  is >> nbRow;
  T TheTol=0;
  WeightMatrix<T,T> WMat=WeightMatrix<T,T>(nbRow,TheTol);
  nbEnt=0;
  for (iRow=0; iRow<nbRow; iRow++)
    for (jRow=0; jRow<nbRow; jRow++) {
      is >> eVal;
      WMat.intDirectAssign(iRow, jRow, eVal);
      if (eVal> nbEnt)
	nbEnt=eVal;
    }
  nbEnt++;
  std::vector<T> ListWeight;
  for (iEnt=0; iEnt<nbEnt; iEnt++) {
    is >> eVal_T;
    ListWeight.push_back(eVal_T);
  }
  WMat.SetWeight(ListWeight);
  return WMat;
}



template<typename T>
void PrintWeightedMatrix(std::ostream &os, WeightMatrix<T,T> const&WMat)
{
  int siz=WMat.GetWeightSize();
  int nbRow=WMat.rows();
  os << "nbRow=" << WMat.rows() << "  nbWeight=" << siz << "\n";
  std::vector<int> ListValues(siz,0);
  for (int iRow=0; iRow<nbRow; iRow++)
    for (int iCol=0; iCol<nbRow; iCol++) {
      int eVal=WMat.GetValue(iRow, iCol);
      ListValues[eVal]++;
    }
  std::vector<T> ListWeight=WMat.GetWeight();
  for (int i=0; i<siz; i++)
    os << "  i=" << i << " nb=" << ListValues[i] << "  eWeight=" << ListWeight[i] << "\n";
  os << "nbRow=" << nbRow << "\n";
  for (int iRow=0; iRow<nbRow; iRow++) {
    for (int iCol=0; iCol<nbRow; iCol++) {
      int eVal=WMat.GetValue(iRow, iCol);
      os << " " << eVal;
    }
    os << "\n";
  }
}



template<typename T>
void PrintWeightedMatrixGAP(std::ostream &os, WeightMatrix<T,T> const&WMat)
{
  std::vector<T> ListWeight=WMat.GetWeight();
  int nbRow=WMat.rows();
  os << "[";
  for (int iRow=0; iRow<nbRow; iRow++) {
    if (iRow > 0)
      os << ",\n";
    os << "[";
    for (int iCol=0; iCol<nbRow; iCol++) {
      int eVal=WMat.GetValue(iRow, iCol);
      T eWei=ListWeight[eVal];
      if (iCol > 0)
	os << ", ";
      os << eWei;
    }
    os << "]";
  }
  os << "]";
}



template<typename T>
void PrintWeightedMatrixNoWeight(std::ostream &os, WeightMatrix<T,T> &WMat)
{
  int siz, nbRow, eVal;
  siz=WMat.GetWeightSize();
  os << "nbWeight=" << siz << "\n";
  nbRow=WMat.rows();
  os << "nbRow=" << WMat.rows() << "\n";
  for (int iRow=0; iRow<nbRow; iRow++) {
    for (int iCol=0; iCol<nbRow; iCol++) {
      eVal=WMat.GetValue(iRow, iCol);
      os << " " << eVal;
    }
    os << "\n";
  }
}






template<typename T1, typename T2, typename Tgr>
inline typename std::enable_if<(not is_functional_graph_class<Tgr>::value),Tgr>::type GetGraphFromWeightedMatrix(WeightMatrix<T1,T2> const& WMat)
{
  int nbWei=WMat.GetWeightSize();
  int nbMult=nbWei+2;
  int hS=GetNeededPower(nbMult);
  //  std::cerr << "nbMult=" << nbMult << " hS=" << hS << "\n";
  int nbRow=WMat.rows();
  int nbVert=nbRow + 2;
  unsigned int nof_vertices=hS*nbVert;
  Tgr eGR(nof_vertices);
  eGR.SetHasColor(true);
  for (int iVert=0; iVert<nbVert; iVert++)
    for (int iH=0; iH<hS; iH++) {
      int aVert=iVert + nbVert*iH;
      eGR.SetColor(aVert,iH);
    }
  for (int iVert=0; iVert<nbVert; iVert++)
    for (int iH=0; iH<hS-1; iH++)
      for (int jH=iH+1; jH<hS; jH++) {
	int aVert=iVert + nbVert*iH;
	int bVert=iVert + nbVert*jH;
	eGR.AddAdjacent(aVert, bVert);
	eGR.AddAdjacent(bVert, aVert);
      }
  for (int iVert=0; iVert<nbVert-1; iVert++)
    for (int jVert=iVert+1; jVert<nbVert; jVert++) {
      int eVal;
      if (jVert == nbRow+1) {
	if (iVert == nbRow)
	  eVal=nbWei;
	else
	  eVal=nbWei+1;
      }
      else {
	if (jVert == nbRow)
	  eVal=WMat.GetValue(iVert, iVert);
	else
	  eVal=WMat.GetValue(iVert, jVert);
      }
      std::vector<int> eVect=GetBinaryExpression(eVal, hS);
      for (int iH=0; iH<hS; iH++)
	if (eVect[iH] == 1) {
	  int aVert=iVert + nbVert*iH;
	  int bVert=jVert + nbVert*iH;
	  eGR.AddAdjacent(aVert, bVert);
	  eGR.AddAdjacent(bVert, aVert);
	}
    }
  return eGR;
}



// This function takes a matrix and returns the vector
// that canonicalize it.
// This depends on the construction of the graph from GetGraphFromWeightedMatrix
// 
template<typename T1, typename T2, typename Tgr>
std::pair<std::vector<int>, std::vector<int>> GetCanonicalizationVector(WeightMatrix<T1,T2> const& WMat)
{
  //  std::cerr << "GetCanonicalizationVector, step 1\n";
  int nbRow=WMat.rows();
  //  std::cerr << "GetCanonicalizationVector, step 2\n";
  Tgr eGR=GetGraphFromWeightedMatrix<T1,T2,Tgr>(WMat);
  //  std::cerr << "GetCanonicalizationVector, step 3\n";
  bliss::Graph g=GetBlissGraphFromGraph(eGR);
  //  std::cerr << "GetCanonicalizationVector, step 4\n";
  int nof_vertices=eGR.GetNbVert();
  bliss::Stats stats;
  const unsigned int* cl;
  cl=g.canonical_form(stats, &report_aut_void, stderr);
  std::vector<int> clR(nof_vertices,-1);
  for (int i=0; i<nof_vertices; i++)
    clR[cl[i]]=i;
  //
  int nbVert=nbRow+2;
  int hS = nof_vertices / nbVert;
#ifdef DEBUG
  if (hS * nbVert != nof_vertices) {
    std::cerr << "Error in the number of vertices\n";
    std::cerr << "hS=" << hS << " nbVert=" << nbVert << " nof_vertices=" << nof_vertices << "\n";
    throw TerminalException{1};
  }
#endif
  //  std::cerr << "GetCanonicalizationVector, step 5 nof_vertices=" << nof_vertices << " hS=" << hS << "\n";
  std::vector<int> MapVect(nbVert, -1), MapVectRev(nbVert,-1);
  std::vector<int> ListStatus(nof_vertices,1);
  int posCanonic=0;
  for (int i=0; i<nof_vertices; i++) {
    if (ListStatus[i] == 1) {
      int iNative=clR[i];
      int iVertNative=iNative % nbVert;
      //      std::cerr << "i=" << i << " nbVert=" << nbRow << " posCanonic=" << posCanonic << " iVertNative=" << iVertNative << "\n";
      MapVectRev[posCanonic] = iVertNative;
      MapVect[iVertNative] = posCanonic;
      for (int iH=0; iH<hS; iH++) {
	int uVertNative = iVertNative + nbVert * iH;
	int j=cl[uVertNative];
#ifdef DEBUG
	if (ListStatus[j] == 0) {
	  std::cerr << "Quite absurd, should not be 0 iH=" << iH << "\n";
	  throw TerminalException{1};
	}
#endif
	ListStatus[j] = 0;
      }
      posCanonic++;
    }
  }
  std::vector<int> MapVect2(nbRow, -1), MapVectRev2(nbRow,-1);
  int posCanonicB=0;
  for (int iCan=0; iCan<nbVert; iCan++) {
    int iNative=MapVectRev[iCan];
    if (iNative < nbRow) {
      MapVectRev2[posCanonicB] = iNative;
      MapVect2[iNative] = posCanonicB;
      posCanonicB++;
    }
  }
  //  std::cerr << "nbRow=" << nbRow << "\n";
  //  std::cerr << "GetCanonicalizationVector, step 6\n";
  return {MapVect2,MapVectRev2};
}




template<typename T1, typename T2, typename Tgr>
inline typename std::enable_if<is_functional_graph_class<Tgr>::value,Tgr>::type GetGraphFromWeightedMatrix(WeightMatrix<T1,T2> const& WMat)
{
  unsigned int nof_vertices;
  int nbMult=WMat.GetWeightSize()+2;
  int hS=GetNeededPower(nbMult);
  int nbRow=WMat.rows();
  int nbVert=nbRow + 2;
  nof_vertices=hS*nbVert;
  std::function<bool(int,int)> fAdj=[=](int const& aVert, int const& bVert) -> bool {
    int eVal;
    int iVert=aVert % nbVert;
    int iH=(aVert - iVert)/nbVert;
    int jVert=bVert % nbVert;
    int jH=(aVert - iVert)/nbVert;
    if (iVert == jVert) {
      if (iH != jH) {
	return true;
      }
      else {
	return false;
      }
    }
    if (iH == jH) {
      if (iVert > jVert) {
	int swp=iVert;
	iVert=swp;
	jVert=swp;
      }
      if (jVert == nbRow+1) {
	if (iVert == nbRow)
	  eVal=nbMult;
	else
	  eVal=nbMult+1;
      }
      else {
	if (jVert == nbRow)
	  eVal=WMat.GetValue(iVert, iVert);
	else
	  eVal=WMat.GetValue(iVert, jVert);
      }
      std::vector<int> eVect=GetBinaryExpression(eVal, hS);
      if (eVect[iH] == 1) {
	return true;
      }
      else {
	return false;
      }
    }
    return false;
  };
  std::function<int(int)> fColor=[=](int const& aVert) -> int {
    int iVert=aVert % nbVert;
    int iH=(aVert - iVert)/nbVert;
    return iH;
  };
  Tgr eGR(nof_vertices, fAdj);
  eGR.SetFColor(fColor);
  return eGR;
}





template<typename T1, typename T2>
TheGroupFormat GetStabilizerWeightMatrix(WeightMatrix<T1, T2> const& WMat)
{
  bliss::Stats stats;
  VectVectInt ListGen;
  VectVectInt *h;
  int nbRow=WMat.rows();
  GraphBitset eGR=GetGraphFromWeightedMatrix<T1,T2,GraphBitset>(WMat);
  /*  std::cerr << "GetStabilizerWeightMatrix, nbRow=" << nbRow << "\n";
  if (nbRow == 64) {
    std::cerr << "We print\n";
    GRAPH_PrintOutputGAP_vertex_colored("GraphExpend.gap", eGR);
    }*/
  bliss::Graph g=GetBlissGraphFromGraph(eGR);
  h=&ListGen;
  g.find_automorphisms(stats, &report_aut_vectvectint, (void *)h);
  int nbGen=ListGen.size();
  std::vector<permlib::Permutation> generatorList;
  for (int iGen=0; iGen<nbGen; iGen++) {
    std::vector<permlib::dom_int> gList(nbRow);
    for (int iVert=0; iVert<nbRow; iVert++) {
      int jVert=ListGen[iGen][iVert];
#ifdef DEBUG
      if (jVert >= nbRow) {
	std::cerr << "jVert is too large\n";
	std::cerr << "jVert=" << jVert << "\n";
	std::cerr << "nbRow=" << nbRow << "\n";
	throw TerminalException{1};
      }
#endif
      gList[iVert]=jVert;
    }
#ifdef DEBUG
    for (int iRow=0; iRow<nbRow; iRow++)
      for (int jRow=0; jRow<nbRow; jRow++) {
	int iRowI=gList[iRow];
	int jRowI=gList[jRow];
	int eVal1=WMat.GetValue(iRow, jRow);
	int eVal2=WMat.GetValue(iRowI, jRowI);
	if (eVal1 != eVal2) {
	  std::cerr << "eVal1=" << eVal1 << " eVal2=" << eVal2 << "\n";
	  std::cerr << "Clear error in automorphism computation\n";
	  std::cerr << "AUT iRow=" << iRow << " jRow=" << jRow << "\n";
	  throw TerminalException{1};
	}
      }
#endif
    generatorList.push_back(permlib::Permutation(gList));
  }
  return GetPermutationGroup(nbRow, generatorList);
}



template<typename T1, typename T2>
EquivTest<permlib::Permutation> TestEquivalenceWeightMatrix_norenorm(WeightMatrix<T1, T2> const& WMat1, WeightMatrix<T1, T2> const& WMat2)
{
  GraphBitset eGR1=GetGraphFromWeightedMatrix<T1,T2,GraphBitset>(WMat1);
  GraphBitset eGR2=GetGraphFromWeightedMatrix<T1,T2,GraphBitset>(WMat2);
  bliss::Graph g1=GetBlissGraphFromGraph(eGR1);
  bliss::Graph g2=GetBlissGraphFromGraph(eGR2);
  int nof_vertices=eGR1.GetNbVert();
  bliss::Stats stats;
  const unsigned int* cl1;
  const unsigned int* cl2;
  cl1=g1.canonical_form(stats, &report_aut_void, stderr);
  cl2=g2.canonical_form(stats, &report_aut_void, stderr);
  std::vector<int> clR2(nof_vertices);
  for (int i=0; i<nof_vertices; i++)
    clR2[cl2[i]]=i;
  std::vector<int> TheEquivExp(nof_vertices, -1);
  for (int iVert=0; iVert<nof_vertices; iVert++)
    TheEquivExp[iVert]=clR2[cl1[iVert]];
  for (int iVert=0; iVert<nof_vertices; iVert++) {
    int jVert=TheEquivExp[iVert];
    if (eGR1.GetColor(iVert) != eGR2.GetColor(jVert))
      return {false, {}};
  }
  for (int iVert1=0; iVert1<nof_vertices; iVert1++) {
    int iVert2=TheEquivExp[iVert1];
    for (int jVert1=0; jVert1<nof_vertices; jVert1++) {
      int jVert2=TheEquivExp[jVert1];
      if (eGR1.IsAdjacent(iVert1,jVert1) != eGR2.IsAdjacent(iVert2,jVert2) )
	return {false, {}};
    }
  }
  int nbRow=WMat1.rows();
  std::vector<permlib::dom_int> TheEquiv(nbRow);
  for (int i=0; i<nbRow; i++)
    TheEquiv[i]=TheEquivExp[i];
#ifdef DEBUG
  for (int iVert1=0; iVert1<nbRow; iVert1++) {
    int iVert2=TheEquiv[iVert1];
    for (int jVert1=0; jVert1<nbRow; jVert1++) {
      int jVert2=TheEquiv[jVert1];
      int eVal1=WMat1.GetValue(iVert1, jVert1);
      int eVal2=WMat2.GetValue(iVert2, jVert2);
      if (eVal1 != eVal2) {
	std::cerr << "nbRow=" << nbRow << "\n";
	std::cerr << "nof_vertices=" << nof_vertices << "\n";
	std::cerr << "iVert1=" << iVert1 << " jVert1=" << jVert1 << "\n";
	std::cerr << "iVert2=" << iVert2 << " jVert2=" << jVert2 << "\n";
	std::cerr << "eVal1=" << eVal1 << " eVal2=" << eVal2 << "\n";
	std::cerr << "Our reduction technique is broken\n";
	std::cerr << "Please panic and debug\n";
	throw TerminalException{1};
      }
    }
  }
#endif
  permlib::Permutation ePerm(TheEquiv);
  return {true, ePerm};
}



template<typename T1, typename T2>
EquivTest<permlib::Permutation> TestEquivalenceWeightMatrix(WeightMatrix<T1, T2> const& WMat1, WeightMatrix<T1, T2> &WMat2)
{
  bool test=RenormalizeWeightMatrix(WMat1, WMat2);
  if (!test)
    return {false, {}};
  return TestEquivalenceWeightMatrix_norenorm(WMat1, WMat2);
}



template<typename T1, typename T2>
EquivTest<permlib::Permutation> TestEquivalenceSubset(WeightMatrix<T1, T2> const& WMat, Face const& f1, Face const& f2)
{
  int siz=WMat.GetWeightSize();
  int n=WMat.rows();
  WeightMatrix<int,int> WMat1(n+1,0);
  WeightMatrix<int,int> WMat2(n+1,0);
  for (int i=0; i<n; i++)
    for (int j=0; j<n; j++) {
      int eVal=WMat.GetValue(i,j);
      WMat1.Update(i,j,eVal);
      WMat2.Update(i,j,eVal);
    }
  for (int i=0; i<n; i++) {
    if (f1[i] == 0)
      WMat1.Update(n,i,siz);
    else
      WMat1.Update(n,i,siz+1);
    if (f2[i] == 0)
      WMat2.Update(n,i,siz);
    else
      WMat2.Update(n,i,siz+1);
  }
  std::vector<int> ListWeight(siz+3);
  for (int i=0; i<siz+3; i++)
    ListWeight[i]=i;
  WMat1.Update(n,n,siz+2);
  WMat2.Update(n,n,siz+2);
  WMat1.SetWeight(ListWeight);
  WMat1.SetWeight(ListWeight);
  EquivTest<permlib::Permutation> test=TestEquivalenceWeightMatrix_norenorm(WMat1, WMat2);
  if (!test.TheReply)
    return {false, {}};
  std::vector<permlib::dom_int> eList(n);
  for (int i=0; i<n; i++) {
    int eVal=test.TheEquiv.at(i);
    eList[i]=eVal;
  }
  return {true, permlib::Permutation(eList)};
}



template<typename T1, typename T2>
TheGroupFormat StabilizerSubset(WeightMatrix<T1, T2> const& WMat, Face const& f)
{
  int siz=WMat.GetWeightSize();
  int n=WMat.rows();
  WeightMatrix<int,int> WMatW(n+1,0);
  for (int i=0; i<n; i++)
    for (int j=0; j<n; j++) {
      int eVal=WMat.GetValue(i,j);
      WMatW.Update(i,j,eVal);
    }
  for (int i=0; i<n; i++) {
    if (f[i] == 0)
      WMatW.Update(n,i,siz);
    else
      WMatW.Update(n,i,siz+1);
  }
  WMatW.Update(n,n,siz+2);
  TheGroupFormat GRP=GetStabilizerWeightMatrix(WMatW);
  std::vector<permlib::Permutation> ListPerm;
  for (auto & ePerm : GRP.group->S) {
    std::vector<permlib::dom_int> eList(n);
    for (int i=0; i<n; i++)
      eList[i]=ePerm->at(i);
    ListPerm.push_back(permlib::Permutation(eList));
  }
  return GetPermutationGroup(n, ListPerm);
}



template<typename T1, typename T2>
WeightMatrix<int,int> NakedWeightedMatrix(WeightMatrix<T1,T2> const& WMat)
{
  int n=WMat.rows();
  WeightMatrix<int,int> WMatNaked(n,0);
  for (int i=0; i<n; i++)
    for (int j=0; j<n; j++) {
      int eVal=WMat.GetValue(i,j);
      WMatNaked.intDirectAssign(i,j,eVal);
    }
  int nbWeight=WMat.GetWeightSize();
  std::vector<int> ListWeight(nbWeight);
  for (int i=0; i<nbWeight; i++)
    ListWeight[i]=i;
  WMatNaked.SetWeight(ListWeight);
  return WMatNaked;
}



template<typename T1, typename T2>
permlib::Permutation CanonicalizeWeightMatrix(WeightMatrix<T1, T2> const& WMat)
{
  GraphBitset eGR=GetGraphFromWeightedMatrix<T1,T2,GraphBitset>(WMat);
  bliss::Graph g=GetBlissGraphFromGraph(eGR);
  int nof_vertices=eGR.GetNbVert();
  bliss::Stats stats;
  const unsigned int* cl=g.canonical_form(stats, &report_aut_void, stderr);
  std::vector<int> clR(nof_vertices);
  for (int i=0; i<nof_vertices; i++)
    clR[cl[i]]=i;
  int nbVert=WMat.GetNbVert();
  std::vector<int> eVect_R(nof_vertices,-1);
  for (int iVert=0; iVert<nbVert; iVert++) {
    int i=cl[iVert]; // or clR? this needs to be debugged
    eVect_R[i]=iVert;
  }
  std::vector<permlib::dom_int> eVect(nbVert);
  int idx=0;
  for (int i=0; i<nof_vertices; i++) {
    int eVal=eVect_R[i];
    if (eVal != -1) {
      eVect[i]=eVal; // Again or reverse? This had to be checked.
      idx++;
    }
  }
  return permlib::Permutation(eVect);
}



template<typename T>
TheGroupFormat LinPolytope_Automorphism(MyMatrix<T> const & EXT)
{
  MyMatrix<T> EXTred=ColumnReduction(EXT);
  WeightMatrix<T,T> WMat=GetWeightMatrix(EXTred);
  return GetStabilizerWeightMatrix(WMat);
}



template<typename Tgr>
int GetNbColor(Tgr const& eGR)
{
  int nbColor=0;
  int nbVert=eGR.GetNbVert();
  for (int iVert=0; iVert<nbVert; iVert++) {
    int eColor=eGR.GetColor(iVert);
    if (eColor > nbColor)
      nbColor=eColor;
  }
  nbColor++;
  return nbColor;
}



template<typename Tgr>
void NAUTY_PrintPartition(std::ostream & os, Tgr const& eGR)
{
  int nbVert=eGR.GetNbVert();
  int nbColor=GetNbColor(eGR);
  os << "f=[";
  for (int iColor=0; iColor<nbColor; iColor++) {
    bool IsFirst=true;
    for (int iVert=0; iVert<nbVert; iVert++) {
      if (!IsFirst) {
	os << " ";
      }
      IsFirst=false;
      if (eGR.GetColor(iVert) == iColor)
	os << iVert;
    }
    if (iColor < nbColor-1)
      os << "|";
  }
  os << "]\n";
}



template<typename Tgr>
void NAUTY_PrintGraph(std::ostream & os, Tgr const& eGR)
{
  os << "g\n";
  int nbVert=eGR.GetNbVert();
  for (int iVert=0; iVert<nbVert; iVert++) {
    os << iVert << " :";
    for (int jVert=0; jVert<nbVert; jVert++)
      if (eGR.IsAdjacent(iVert, jVert))
	os << " " << jVert;
    os << ";\n";
  }
}



template<typename T, typename Tgr>
void NAUTY_AUTO_WriteFile(std::ostream & os, std::vector<MyMatrix<T> > const& ListMatrix, MyMatrix<T> const& SHV)
{
  MyMatrix<T> T_SHV=TMat_ConvertFromInt<T>(SHV);
  WeightMatrix<std::vector<T>, T> ScalMat = GetWeightMatrix_ListMatrix(ListMatrix, T_SHV);
  Tgr eGR=GetGraphFromWeightedMatrix<std::vector<T>, T, Tgr>(ScalMat);
  int nbVert=eGR.nbVertices;
  os << "n=" << nbVert << "\n";
  NAUTY_PrintPartition(os, eGR);
  NAUTY_PrintGraph(os, eGR);
}



template<typename T, typename Tgr>
bool NAUTY_ISOM_WriteFile(std::ostream & os,
			 std::vector<MyMatrix<T> > const& ListMatrix1, MyMatrix<T> const& SHV1,
			 std::vector<MyMatrix<T> > const& ListMatrix2, MyMatrix<T> const& SHV2)
{
  MyMatrix<T> T_SHV1=TMat_ConvertFromInt<T>(SHV1);
  MyMatrix<T> T_SHV2=TMat_ConvertFromInt<T>(SHV2);
  WeightMatrix<std::vector<T>, T> ScalMat1 = GetWeightMatrix_ListMatrix(ListMatrix1, T_SHV1);
  WeightMatrix<std::vector<T>, T> ScalMat2 = GetWeightMatrix_ListMatrix(ListMatrix2, T_SHV2);
  bool test=RenormalizeWeightMatrix(ScalMat1, ScalMat2);
  if (!test)
    return false;
  Tgr eGR1=GetGraphFromWeightedMatrix<std::vector<T>, T, Tgr>(ScalMat1);
  Tgr eGR2=GetGraphFromWeightedMatrix<std::vector<T>, T, Tgr>(ScalMat2);
  int nbVert=eGR1.GetNbVert();
  os << "n=" << nbVert << "\n";
  NAUTY_PrintPartition(os, eGR1);
  NAUTY_PrintGraph(os, eGR1);
  os << "c x @\n";
  NAUTY_PrintGraph(os, eGR2);
  os << "x ##\n";
  return true;
}



template<typename Tgr>
TheGroupFormat GRAPH_Automorphism_Nauty(Tgr const& eGR)
{
  std::string ePrefix=random_string(20);
  TempFile eFileIn ("/tmp/NAUTY_AUTO_" + ePrefix + ".inp");
  TempFile eFileOut("/tmp/NAUTY_AUTO_" + ePrefix + ".out");
  TempFile eFileErr("/tmp/NAUTY_AUTO_" + ePrefix + ".err");
  TempFile eFileGrp("/tmp/NAUTY_AUTO_" + ePrefix + ".grp");
  //
  std::ofstream os(eFileIn.string());
  int nbVert=eGR.GetNbVert();
  os << "n=" << nbVert << "\n";
  NAUTY_PrintPartition(os, eGR);
  NAUTY_PrintGraph(os, eGR);
  os.close();
  //
  std::string FileDR2="dreadnaut";
  std::string TheCommand=FileDR2 + " < " + eFileIn.string() + " > " + eFileOut.string() + " 2> " + eFileErr.string();
  int iret=system(TheCommand.c_str());
  if (iret == -1) {
    std::cerr << "unable to run the command\n";
    throw TerminalException{1};
  }
  if (!eFileOut.IsExisting()) {
    std::cerr << "Error while opening file\n";
    throw TerminalException{1};
  }
  //
  std::string FileConvert="NautyGroupToCPP";
  TheCommand=FileConvert + " " + eFileOut.string() + " " + IntToString(nbVert) + " > " + eFileGrp.string();
  //
  std::ifstream is(eFileGrp.string());
  TheGroupFormat GRP=ReadGroup(is);
  //
  return GRP;
}



template<typename Tgr>
EquivTest<permlib::Permutation> GRAPH_Isomorphism_Nauty(Tgr const& eGR1, Tgr const& eGR2)
{
  std::string ePrefix=random_string(20);
  TempFile eFileIn ("/tmp/NAUTY_ISOM_" + ePrefix + ".inp");
  TempFile eFileOut("/tmp/NAUTY_ISOM_" + ePrefix + ".out");
  TempFile eFileErr("/tmp/NAUTY_ISOM_" + ePrefix + ".err");
  TempFile eFileIso("/tmp/NAUTY_ISOM_" + ePrefix + ".iso");
  //
  if (eGR1.GetNbVert() != eGR2.GetNbVert() || GetNbColor(eGR1) != GetNbColor(eGR2))
    return {false, {}};
  //
  bool test;
  std::ofstream os(eFileIn.string());
  int nbVert=eGR1.GetNbVert();
  os << "n=" << nbVert << "\n";
  NAUTY_PrintPartition(os, eGR1);
  NAUTY_PrintGraph(os, eGR1);
  os << "c x @\n";
  NAUTY_PrintGraph(os, eGR2);
  os << "x ##\n";
  os.close();
  //
  std::string FileDR2="dreadnaut";
  std::string TheCommand=FileDR2 + " < " + eFileIn.string() + " > " + eFileOut.string() + " 2> " + eFileErr.string();
  int iret=system(TheCommand.c_str());
  if (iret == -1) {
    std::cerr << "unable to run the command\n";
    throw TerminalException{1};
  }
  if (!eFileOut.IsExisting()) {
    std::cerr << "Error while opening file\n";
    throw TerminalException{1};
  }
  //
  std::string FileConvert="NautyGroupToCPP";
  TheCommand=FileConvert + " " + eFileOut.string() + " > " + eFileIso.string();
  //
  std::ifstream is(eFileIso.string());
  is >> test;
  if (test == 0) {
    return {false, {}};
  }
  std::vector<permlib::dom_int> eList(nbVert);
  for (int iVert=0; iVert<nbVert; iVert++) {
    int eVal;
    is >> eVal;
    eList[iVert]=eVal;
  }
  permlib::Permutation ePerm(eList);
  return {true, ePerm};
}



template<typename T, typename Tgr>
EquivTest<permlib::Permutation> LinPolytopeGram_Isomorphism_Nauty(std::vector<MyMatrix<T> > const& ListMatrix1, MyMatrix<int> const& SHV1, std::vector<MyMatrix<T> > const& ListMatrix2, MyMatrix<int> const& SHV2)
{
  MyMatrix<T> T_SHV1=TMat_ConvertFromInt<T>(SHV1);
  MyMatrix<T> T_SHV2=TMat_ConvertFromInt<T>(SHV2);
  WeightMatrix<std::vector<T>, T> ScalMat1 = GetWeightMatrix_ListMatrix(ListMatrix1, T_SHV1);
  WeightMatrix<std::vector<T>, T> ScalMat2 = GetWeightMatrix_ListMatrix(ListMatrix2, T_SHV2);
  int nbVert=SHV1.rows();
  bool test=RenormalizeWeightMatrix(ScalMat1, ScalMat2);
  if (!test)
    return {false, {}};
  Tgr eGR1=GetGraphFromWeightedMatrix<std::vector<T>, T, Tgr>(ScalMat1);
  Tgr eGR2=GetGraphFromWeightedMatrix<std::vector<T>, T, Tgr>(ScalMat2);
  EquivTest<permlib::Permutation> eRes=GRAPH_Isomorphism_Nauty(eGR1, eGR2);
  if (!eRes.TheReply)
    return {false, {}};
  std::vector<permlib::dom_int> eListRed(nbVert);
  for (int i=0; i<nbVert; i++)
    eListRed[i]=eRes.TheEquiv.at(i);
  return {true, permlib::Permutation(eListRed)};
}



template<typename T, typename Tgr>
TheGroupFormat LinPolytopeGram_Automorphism_Nauty(std::vector<MyMatrix<T> > const& ListMatrix, MyMatrix<int> const& SHV)
{
  int nbVert=SHV.rows();
  MyMatrix<T> T_SHV=TMat_ConvertFromInt<T>(SHV);
  WeightMatrix<std::vector<T>, T> ScalMat = GetWeightMatrix_ListMatrix(ListMatrix, T_SHV);
  Tgr eGR=GetGraphFromWeightedMatrix<std::vector<T>, T, Tgr>(ScalMat);
  TheGroupFormat GRP=GRAPH_Automorphism_Nauty(eGR);
  //
  std::vector<permlib::Permutation> ListGen;
  for (auto & eGen : GRP.group->S) {
    std::vector<permlib::dom_int> v(nbVert);
    for (int iVert=0; iVert<nbVert; iVert++) {
      int jVert=eGen->at(iVert);
      v[iVert]=jVert;
    }
    ListGen.push_back(permlib::Permutation(v));
  }
  return GetPermutationGroup(nbVert, ListGen);
}



template<typename T>
MyMatrix<T> RepresentVertexPermutation(MyMatrix<T> const& EXT1,
				       MyMatrix<T> const& EXT2,
				       permlib::Permutation const& ePerm)
{
  SelectionRowCol<T> eSelect=TMat_SelectRowCol(EXT1);
  std::vector<int> ListRowSelect=eSelect.ListRowSelect;
  MyMatrix<T> M1=SelectRow(EXT1, ListRowSelect);
  MyMatrix<T> M1inv=Inverse(M1);
  int nbRow=ListRowSelect.size();
  std::vector<int> ListRowSelectImg(nbRow);
  for (int iRow=0; iRow<nbRow; iRow++) {
    int jRow=ePerm.at(iRow);
    ListRowSelectImg[iRow]=jRow;
  }
  MyMatrix<T> M2=SelectRow(EXT2, ListRowSelectImg);
  return M1inv*M2;
}



template<typename T>
permlib::Permutation GetPermutationOnVectors(MyMatrix<T> const& EXT1,
					     MyMatrix<T> const& EXT2)
{
  int nbVect=EXT1.rows();
  std::vector<MyVector<T>> EXTrow1(nbVect), EXTrow2(nbVect);
  for (int iVect=0; iVect<nbVect; iVect++) {
    MyVector<T> eRow1=GetMatrixRow(EXT1, iVect);
    EXTrow1[iVect]=eRow1;
    //
    MyVector<T> eRow2=GetMatrixRow(EXT2, iVect);
    EXTrow2[iVect]=eRow2;
  }
  permlib::Permutation ePerm1=SortingPerm(EXTrow1);
  permlib::Permutation ePerm2=SortingPerm(EXTrow2);
  permlib::Permutation ePermRet=(~ePerm1) * ePerm2;
#ifdef DEBUG
  for (int iVect=0; iVect<nbVect; iVect++) {
    int jVect=ePermRet.at(iVect);
    if (EXTrow2[jVect] != EXTrow1[iVect]) {
      std::cerr << "iVect=" << iVect << " jVect=" << jVect << "\n";
      std::cerr << "Id:";
      for (int k=0; k<nbVect; k++)
	std::cerr << " " << k;
      std::cerr << "\n";
      std::cerr << " p:";
      for (int k=0; k<nbVect; k++)
	std::cerr << " " << ePermRet.at(k);
      std::cerr << "\n";

      std::cerr << "perm1:";
      for (int k=0; k<nbVect; k++)
	std::cerr << " " << ePerm1.at(k);
      std::cerr << "\n";
      std::cerr << "perm2:";
      for (int k=0; k<nbVect; k++)
	std::cerr << " " << ePerm2.at(k);
      std::cerr << "\n";


      std::cerr << "EXTrow1[iVect]=";
      WriteVector(std::cerr, EXTrow1[iVect]);
      std::cerr << "EXTrow2[jVect]=";
      WriteVector(std::cerr, EXTrow2[jVect]);
      std::cerr << "EXTrow1=\n";
      WriteMatrix(std::cerr, EXT1);
      std::cerr << "EXTrow2=\n";
      WriteMatrix(std::cerr, EXT2);
      std::cerr << "Error in GetPermutationOnVectors\n";
      throw TerminalException{1};
    }
  }
#endif
  return ePermRet;
}



std::vector<Face> OrbitSplittingSet(std::vector<Face> const& PreListTotal, 
				    TheGroupFormat const& TheGRP)
{
  std::vector<Face> TheReturn;
  std::set<Face> ListTotal;
  for (auto eFace : PreListTotal)
    ListTotal.insert(eFace);
  while(true) {
    std::set<Face>::iterator iter=ListTotal.begin();
    if (iter == ListTotal.end())
      break;
    Face eSet=*iter;
    TheReturn.push_back(eSet);
    std::set<Face> Additional{eSet};
    ListTotal.erase(eSet);
    std::set<Face> SingleOrbit;
    while(true) {
      std::set<Face> NewElts;
      for (auto const& gSet : Additional)
	for (auto const& eGen : TheGRP.group->S) {
	  Face fSet=eEltImage(gSet, *eGen);
	  if (SingleOrbit.find(fSet) == SingleOrbit.end() && Additional.find(fSet) == Additional.end()) {
	    if (NewElts.find(fSet) == NewElts.end()) {
	      if (ListTotal.find(fSet) != ListTotal.end()) {
		NewElts.insert(fSet);
		ListTotal.erase(fSet);
	      }
#ifdef DEBUG
	      else {
		std::cerr << "Orbit do not matched, PANIC!!!\n";
		throw TerminalException{1};
	      }
#endif
	    }
	  }
	}
      for (auto & uSet : Additional)
	SingleOrbit.insert(uSet);
      if (NewElts.size() == 0)
	break;
      Additional=NewElts;
    }
  }
  return TheReturn;
}



template<typename Tobj, typename Tgen>
std::vector<Tobj> OrbitSplittingGeneralized(std::vector<Tobj> const& PreListTotal,
					    std::vector<Tgen> const& ListGen,
					    std::function<Tobj(Tobj const&,Tgen const&)> const& TheAct)
{
  std::vector<Tobj> TheReturn;
  std::set<Tobj> ListTotal;
  for (auto eObj : PreListTotal)
    ListTotal.insert(eObj);
  while(true) {
    auto iter=ListTotal.begin();
    if (iter == ListTotal.end())
      break;
    Tobj eObj=*iter;
    TheReturn.push_back(eObj);    
    std::set<Tobj> Additional;
    Additional.insert(eObj);
    ListTotal.erase(eObj);
    std::set<Tobj> SingleOrbit;
    while(true) {
      std::set<Tobj> NewElts;
      for (auto const& gObj : Additional)
	for (auto const& eGen : ListGen) {
	  Tobj fObj=TheAct(gObj, eGen);
	  if (SingleOrbit.find(fObj) == SingleOrbit.end() && Additional.find(fObj) == Additional.end()) {
	    if (NewElts.find(fObj) == NewElts.end()) {
	      if (ListTotal.find(fObj) != ListTotal.end()) {
		NewElts.insert(fObj);
		ListTotal.erase(fObj);
	      }
#ifdef DEBUG
	      else {
		std::cerr << "Orbit do not match, PANIC!!!\n";
		throw TerminalException{1};
	      }
#endif
	    }
	  }
	}
      for (auto & uSet : Additional)
	SingleOrbit.insert(uSet);
      if (NewElts.size() == 0)
	break;
      Additional=NewElts;
    }
  }
  return TheReturn;
}



std::vector<Face> DoubleCosetDescription(TheGroupFormat const& BigGRP,
					 TheGroupFormat const& SmaGRP,
					 LocalInvInfo const& LocalInv, 
					 Face const& eList, std::ostream & os)
{
  os << "Beginning of DoubleCosetDescription\n";
  std::list<permlib::Permutation::ptr> ListGen=BigGRP.group->S;
  TheGroupFormat TheStab=GetStabilizer(BigGRP, eList);
  os << "BigGRP.size=" << BigGRP.size << " TheStab.size=" << TheStab.size << "\n";
  mpz_class TotalSize=BigGRP.size / TheStab.size;
  os << "TotalSize=" << TotalSize << "\n";
  //
  struct Local {
    int status;
    Face eFace;
    std::vector<int> eInv;
  };  
  mpz_class SizeGen=0;
  std::vector<Local> ListLocal;
  auto DoubleCosetInsertEntry=[&](Face const& testList) -> void {
    std::vector<int> eInv=GetLocalInvariantWeightMatrix_Enhanced<int>(LocalInv, testList);
    for (auto const& fLocal : ListLocal) {
      bool testCL=TestEquivalenceGeneralNaked(SmaGRP, fLocal.eFace, testList, 0).TheReply;
      bool testPA=TestEquivalenceGeneralNaked(SmaGRP, fLocal.eFace, testList, 1).TheReply;      
      bool test=TestEquivalence(SmaGRP, fLocal.eFace, testList);
      os << "fLocal.eFace=\n";
      WriteFace(os, fLocal.eFace);
      os << "    testList=\n";
      WriteFace(os, testList);
      os << "fLocal.eFace=" << fLocal.eFace << "\n";
      os << "    testList=" << testList << "  testCL=" << testCL << " testPA=" << testPA << "\n";
      if (test)
	return;
    }
    ListLocal.push_back({0,testList,eInv});
    TheGroupFormat fStab=GetStabilizer(SmaGRP, testList);
    os << "SmaGRP.size=" << SmaGRP.size << " fStab.size=" << fStab.size << "\n";
    mpz_class OrbSizeSma=SmaGRP.size / fStab.size;
    SizeGen += OrbSizeSma;
    os << "Now SizeGen=" << SizeGen << " OrbSizeSma=" << OrbSizeSma << " |ListLocal|=" << ListLocal.size() << "\n";
  };
  DoubleCosetInsertEntry(eList);
  while(true) {
    bool DoSomething=false;
    int nbLocal=ListLocal.size();
    for (int iLocal=0; iLocal<nbLocal; iLocal++) 
      if (ListLocal[iLocal].status == 0) {
	ListLocal[iLocal].status=1;
	DoSomething=true;
	int iGen=0;
	Face eFace=ListLocal[iLocal].eFace;
	for (auto const& eGen : ListGen) {
	  //	  permlib::Permutation eGenB=*eGen;
	  //	  std::cerr << "iGen=" << iGen << " eGenB.size=" << eGenB.size() << " |eSet|s=" << eFace.size() << " |eSet|c=" << eFace.count() << "\n";
	  Face eNewList=eEltImage(eFace, *eGen);
	  DoubleCosetInsertEntry(eNewList);
	  iGen++;
	}
      }
    if (!DoSomething)
      break;
  }
  os << "After Iteration loop SizeGen=" << SizeGen << " TotalSize=" << TotalSize << "\n";
  std::vector<Face> ListListSet;
  for (auto & eRec : ListLocal)
    ListListSet.push_back(eRec.eFace);
  if (SizeGen == TotalSize)
    return ListListSet;
  std::vector<Face> PartialOrbit=ListListSet;
  auto IsPresent=[&](Face const& testList) -> bool {
    for (auto & fList : PartialOrbit)
      if (fList == testList)
	return true;
    return false;
  };
  while(true) {
    for (auto & eGen : ListGen) {
      int len=PartialOrbit.size();
      for (int i=0; i<len; i++) {
	Face eNewList=eEltImage(PartialOrbit[i], *eGen);
	if (!IsPresent(eNewList)) {
	  PartialOrbit.push_back(eNewList);
	  DoubleCosetInsertEntry(eNewList);
	  if (SizeGen == TotalSize) {
	    std::vector<Face> ListListFin;
	    for (auto & eRec : ListLocal)
	      ListListFin.push_back(eRec.eFace);
	    return ListListFin;
	  }
	}
      }
    }
  }
  os << "Likely not reachable stage\n";
  throw TerminalException{1};
}



std::vector<Face> OrbitSplittingListOrbit(TheGroupFormat const& BigGRP, TheGroupFormat const& SmaGRP, std::vector<Face> eListBig, std::ostream & os)
{
  os << "|BigGRP|=" << BigGRP.size << " |SmaGRP|=" << SmaGRP.size << "\n";
  if (BigGRP.size == SmaGRP.size)
    return eListBig;
  {
    std::ofstream os1("ORBSPLIT_BigGRP");
    std::ofstream os2("ORBSPLIT_BigGRP.gap");
    std::ofstream os3("ORBSPLIT_SmaGRP");
    std::ofstream os4("ORBSPLIT_SmaGRP.gap");
    std::ofstream os5("ORBSPLIT_ListBig");
    std::ofstream os6("ORBSPLIT_ListBig.gap");
    WriteGroup      (os1, BigGRP);
    WriteGroupGAP   (os2, BigGRP);
    WriteGroup      (os3, SmaGRP);
    WriteGroupGAP   (os4, SmaGRP);
    WriteListFace   (os5, eListBig);
    WriteListFaceGAP(os6, eListBig);
  }
  WeightMatrix<int,int> WMat=WeightMatrixFromPairOrbits<int,int>(SmaGRP, os);
  LocalInvInfo LocalInv=ComputeLocalInvariantStrategy(WMat, SmaGRP, "pairinv", os);
  os << "We do the algorithm\n";
  std::vector<Face> eListSma;
  int iter=0;
  for (auto & eSet : eListBig) {
    os << "iter=" << iter << " Before DoubleCosetDescription\n";
    std::vector<Face> ListListSet=DoubleCosetDescription(BigGRP, SmaGRP, LocalInv, eSet, os);
    os << "      |ListListSet|=" << ListListSet.size() << "\n";
    for (auto & eCos : ListListSet)
      eListSma.push_back(eCos);
    os << "      |eListSma|=" << eListSma.size() << "\n";
    iter++;
  }
  return eListSma;
}



#endif

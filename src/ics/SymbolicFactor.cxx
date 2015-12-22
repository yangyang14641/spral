#include "SymbolicFactor.hxx"

#include <stdexcept>

#include "AssemblyTree.hxx"
#include "Chunk.hxx"
#include "spral_metis_wrapper.h"

namespace spral {
namespace ics {

/* Constructs symbolic factorization from matrix data */
SymbolicFactor::SymbolicFactor (int n, int ptr[], int row[], int nemin) :
      nemin(nemin), nfact(0), nflop(0), n_(n), nnodes_(0), perm_(nullptr),
      factor_mem_size_(0) {

   /* Perform METIS ordering */
   perm_ = new int[n];
   int *invp = new int[n];
   int flag = spral_metis_order(n, ptr, row, perm_, invp, 0);
   delete[] invp;
   if(flag)
      throw std::runtime_error("spral_metis_order() failed");

   /* Construct AssemblyTree */
   AssemblyTree tree(n, ptr, row, perm_, nemin);

   /* Construct chunk buckets */
   const int MAXROW=50;
   const int MAXCOL=8;
   int clen[MAXROW+1][MAXCOL+1];
   for(int i=0; i<MAXROW+1; i++)
   for(int j=0; j<MAXCOL+1; j++)
      clen[i][j] = 0;
   for(auto nitr=tree.leaf_first_begin(); nitr!=tree.leaf_first_end(); ++nitr) {

      const AssemblyTree::Node node = *nitr;
      int i = (node.get_nrow()-1)/4;
      if(i>MAXROW) i = MAXROW;
      int j = node.get_ncol()-1;
      if(j>MAXCOL) j = MAXCOL;
      if(i==MAXROW || j==MAXCOL)
         printf("Node %d is %d x %d\n", node.idx, node.get_nrow(), node.get_ncol());
      clen[i][j]++;
   }
   printf("Buckets:\n  ");
   for(int i=0; i<MAXCOL; i++) printf(" %4d", i+1);
   printf("   >%d\n", MAXCOL);
   for(int i=0; i<MAXROW+1; i++) {
      printf("%2d", i+1);
      for(int j=0; j<MAXCOL+1; j++)
         printf(" %4d", clen[i][j]);
      printf("\n");
   }

}

} /* namespace ics */
} /* namespace spral */

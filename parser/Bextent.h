#ifndef BEXTENT_H
#define BEXTENT_H

#include <Bprojlist.h>

enum P_Bool {P_False, P_True};

class Extent {
public:
     // Selection on all the tuples of the extent.
     int select(const char*     extentName,
		Aptree_t& predicatetree,
		const char*     resExtentName,
		P_Bool    tmpFlag,
		const char*     attrSuffix,
		ProjList& projList,
		P_Bool    isAggregate = P_False);

     // Selection using an index.
     int indexSelect(const char*     extentName,
		     const char*     attrName,
		     Aptree_t& idxPredTree,
		     Aptree_t& auxPredTree,
		     const char*     resExtentName,
		     P_Bool    tmpFlag,
		     const char*     attrSuffix,
		     ProjList& projList);

     // Block Nested-loops join
     int nlJoin(const char*     outerExtentName,
		const char*     innerExtentName,
		Aptree_t& predicatetree,
		const char*     resExtentName,
		P_Bool    tmpFlag,
                const char*     outerAttrSuffix,
                const char*     innerAttrSuffix,
		ProjList& projList);
   
     // Nested-loops indexed join
     int nlIdxJoin(const char*     outerExtentName,
		   const char*     innerExtentName,
		   Aptree_t& idxPredTree,
		   Aptree_t& auxPredTree,
		   const char*     resExtentName,
		   P_Bool    tmpFlag,
		   const char*     outerAttrSuffix,
		   const char*     innerAttrSuffix,
		   ProjList& projList);
     int mergeJoin(const char*     outerExtentName,
		   const char*     innerExtentName,
		   Aptree_t& mergePredTree,
		   Aptree_t& auxPredTree,
		   const char*     resExtentName,
		   P_Bool    tmpFlag,
		   const char*     outerAttrSuffix,
		   const char*     innerAttrSuffix,
		   ProjList& projList);
     int hashJoin(const char*     outerExtentName,
		  const char*     innerExtentName,
		  Aptree_t& hashPredTree,
		  Aptree_t& auxPredTree,
		  const char*     resExtentName,
		  P_Bool    tmpFlag,
		  const char*     outerAttrSuffix,
		  const char*     innerAttrSuffix,
		  ProjList& projList);
   
   // The sort operator.
   int sortExtent(const char*     extentName,
		  const char*     attrName,
		  P_Bool    eliminateDup,
		  const char*     resExtentName,
		  P_Bool    tmpFlag,
		  ProjList &projList);
   
   int aggregate(const char*     extentName,    
		 Aptree_t& ppPredTree,    
		 const char*     resExtentName, 
		 P_Bool    tmpFlag,       
		 const char*     attrSuffix,
		 ProjList& projList,
		 const char*     groupBy[]      = 0,     
		 int       numGroupByAttr = 0);
   int update (const char *extentName,
	       Aptree_t& ppPredTree,
	       ProjList &projList);
   int insert (const char *extentName,
	       ProjList &projList);

};

#endif /* BEXTENT_H */

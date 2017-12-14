#ifndef ABITMAP_H
#define ABITMAP_H

#include <limits.h>

typedef unsigned int chunk_t;
const int chunkbits = (sizeof (chunk_t)*CHAR_BIT);

class Abitmap_t {
protected:

private:
  chunk_t *bitmap;
  
protected:
  inline static int NumberOfChunks (int numberofbits) {
    return (numberofbits>0 ?
		((numberofbits-1)/chunkbits + 1) : 1);
  }
  inline static int BitPosition (int bitnumber) {
    return bitnumber%chunkbits;
  }
  inline static int ChunkNumber (int bitnumber) {
    return bitnumber/chunkbits;
  }
  inline static chunk_t UpperMask (int numberofbits) {
    int bitpos = BitPosition (numberofbits);
    if (bitpos == 0) return ~(chunk_t)0;
    else return ~(~(chunk_t)0 << bitpos);
  }

public:
  Abitmap_t (int numberofbits);
  Abitmap_t (const Abitmap_t &, int numberofbits);
  ~Abitmap_t (void) {delete [] bitmap;}

  void Copy (const Abitmap_t &other, int numberofbits);

  void TurnOn (int bitnumber) {
    bitmap[ChunkNumber (bitnumber)] |= 1 << BitPosition (bitnumber);
  }
  void TurnOff (int bitnumber) {
    bitmap[ChunkNumber(bitnumber)] &= ~(1 << BitPosition (bitnumber));
  }
  int IsOn (int bitnumber) const {
    return bitmap[ChunkNumber (bitnumber)] & (1 << BitPosition (bitnumber));
  }
  
  void Or (const Abitmap_t &, int numberofbits);
  void And (const Abitmap_t &, int numberofbits);
  void Minus (const Abitmap_t &, int numberofbits);
  void Not (int numberofbits);
  void Clear (int numberofbits);

  int IsSubsetOf (const Abitmap_t &, int numberofbits) const;

  int IsEqualTo (const Abitmap_t &, int numberofbits) const;

  int IsEmpty (int numberofbits) const;

  int NumberOfBitsOn (int totalnumberofbits) const;

  int AreAllBitsOn (int numberofbits) const;

  // return the position of the first bit which is ON
  int FirstBit (int numberofbits) const;

  // a hash value based on the logical properties.
  // guaranteed to be equal for two bitmap objects
  // if they are equal according to the Abitmap_t::IsEqualTo 
  int Hash (int numberofbits) const; 
};

/***************************************************************************

  inline functions

***************************************************************************/

#endif /* ABITMAP_H */

#include <string.h>
#include "Abitmap.h"

Abitmap_t::Abitmap_t (int numberofbits)
{
  int chunknumber = NumberOfChunks (numberofbits);
  bitmap = new chunk_t [chunknumber];
  for (chunknumber--; chunknumber >= 0; chunknumber--)
    bitmap[chunknumber] = (chunk_t)0;
}

Abitmap_t::Abitmap_t (const Abitmap_t &newbitmap, int numberofbits)
{
  bitmap = new chunk_t [NumberOfChunks (numberofbits)];
  Copy (newbitmap, numberofbits);
}

void
Abitmap_t::Copy (const Abitmap_t &newbitmap, int numberofbits)
{
  int chunknumber = NumberOfChunks (numberofbits);
  for (chunknumber--; chunknumber >= 0; chunknumber--)
    bitmap[chunknumber] = newbitmap.bitmap[chunknumber];
}
  

void
Abitmap_t::And (const Abitmap_t &otherbitmap, int numberofbits)
{
  int numberofchunks = NumberOfChunks (numberofbits);
  for (int i=0; i < numberofchunks; i++)
    bitmap[i] &= otherbitmap.bitmap[i];
}

void
Abitmap_t::Or (const Abitmap_t &otherbitmap, int numberofbits)
{
  int numberofchunks = NumberOfChunks (numberofbits);
  for (int i=0; i < numberofchunks; i++)
    bitmap[i] |= otherbitmap.bitmap[i];
}

void
Abitmap_t::Minus (const Abitmap_t &otherbitmap, int numberofbits)
{
  int numberofchunks = NumberOfChunks (numberofbits);
  for (int i=0; i < numberofchunks; i++)
    bitmap[i] = bitmap[i] & ~otherbitmap.bitmap[i];
}

void
Abitmap_t::Not (int numberofbits)
{
  int numberofchunks = NumberOfChunks (numberofbits);
  for (int i=0; i < numberofchunks; i++)
    bitmap[i] = ~bitmap[i];

  bitmap[numberofchunks-1] &= UpperMask(numberofbits);
		// we have to always ensure that the unused bits
		// in the last chunk are always zero.
}

void
Abitmap_t::Clear (int numberofbits)
{
  int chunknumber = NumberOfChunks (numberofbits);
  for (chunknumber--; chunknumber >= 0; chunknumber--)
    bitmap[chunknumber] = (chunk_t)0;
}

int
Abitmap_t::IsSubsetOf (const Abitmap_t &otherbitmap, int numberofbits) const
{
  int numberofchunks = NumberOfChunks (numberofbits);
  for (int i=0; i < numberofchunks; i++)
    if ((bitmap[i] | otherbitmap.bitmap[i]) != otherbitmap.bitmap[i])
      return 0; // is not a subset
  return 1; // yes it is.
}

int
Abitmap_t::IsEqualTo (const Abitmap_t &otherbitmap, int numberofbits) const
{
  if (memcmp (bitmap, otherbitmap.bitmap, 
	      sizeof (chunk_t)*NumberOfChunks (numberofbits)))
    return 0;
  else
    return 1;
}

int 
Abitmap_t::NumberOfBitsOn (int totalnumberofbits) const
{
  int numberofbitson = 0;
  for (int i=0; i < totalnumberofbits; i++)
    if (IsOn (i))
      numberofbitson++;
  return numberofbitson;
}

int 
Abitmap_t::FirstBit (int numberofbits) const
{
  for (int i=0; i < numberofbits; i++)
    if (IsOn (i))
      return i;
  return -1;
}

// a hash value based on the logical properties.
// guaranteed to be equal for two bitmap objects
// if they are equal according to the Abitmap_t::IsEqualTo 
int
Abitmap_t::Hash (int numberofbits) const
{
  int numberofchunks = NumberOfChunks (numberofbits);
  int hashvalue = 0;
  for (int i=0; i < numberofchunks; i++)
    hashvalue += ((i+1)*(int)bitmap[i] + (int)bitmap[i] << 6);
  return hashvalue;
}

int Abitmap_t::IsEmpty (int numberofbits) const
{
  int numberofchunks = NumberOfChunks (numberofbits);
  for (int i=0; i < numberofchunks; i++)
    if (bitmap[i] != (chunk_t) 0)
      return 0; // no it is not empty
  return 1; // yes it is.
}


int Abitmap_t::AreAllBitsOn (int numberofbits) const
{
  int numberofchunks = NumberOfChunks (numberofbits);
  for (int i=0; i < numberofchunks-1; i++)
    if (~bitmap[i] != (chunk_t)0)
      return 0;			// no all bits are not on

  return bitmap[numberofchunks-1] == UpperMask(numberofbits);
				// checking for the last chunk is a
				// litle tricky.
}

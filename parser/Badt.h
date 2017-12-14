#ifndef BADT_H
#define BADT_H

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <iostream.h>

class Badt_t {
private:
  char *_adt_val;
  int _size;
public:
  Badt_t (void) {_adt_val = 0; _size = 0;}
  virtual ~Badt_t (void) {}
  void init (int sz, void *val) {_size = sz; _adt_val = (char *)val;}
  virtual void delete_data (void) {delete [] _adt_val; _adt_val=0; _size=0;}
  virtual void ascii_to_mem (const char *str) = 0;
  virtual double get_numeric_val (void) const {assert (!"not numeric type");}
  virtual print (ostream &os) const {assert (!"printing unknown adt");}
  char *adt_val (void) const {return _adt_val;}
  int size (void) const {return _size;}
};

class Binteger_t : public Badt_t {
private:
  int _int_val;
public:
  void init (int i) {_int_val = i; Badt_t::init (sizeof (i), &i);} 
  virtual void delete_data (void) {} // no deletion required
  virtual void ascii_to_mem (const char *str) {init (atoi (str));}
  int int_val (void) const {return _int_val;}
  virtual double get_numeric_val (void) const {return (double)int_val ();}
  virtual print (ostream &os) const {os << int_val ();}
};

class Breal_t : public Badt_t {
private:
  float _real_val;
public:
  void init (float f) {_real_val = f; Badt_t::init (sizeof (f), &f);}
  virtual void delete_data (void) {} // no deletion required
  virtual void ascii_to_mem (const char *str) {init (atof (str));}
  float real_val (void) const {return _real_val;}
  virtual double get_numeric_val (void) const {return (double)real_val ();}
  virtual print (ostream &os) const {os << real_val ();}
};

class Bstring_t : public Badt_t {
public:
  char *string_val (void) const {return adt_val ();}
  virtual void ascii_to_mem (const char *str);
  int string_len (void) const {return size ();}
  virtual print (ostream &os) const {os << string_val ();}
};

/***************************************************************************

  inline functions

***************************************************************************/
inline void Bstring_t::ascii_to_mem (const char *str)
{
  int len = strlen (str); 
  char *data = new char[len+1]; assert (data);
  strcpy (data, str);
  init (len, data);
}
#endif /* BADT_H */



#ifndef ATIMER_H
#define ATIMER_H


#include <sys/time.h>
#include <sys/resource.h>
#include <assert.h>
#include <iostream.h>
#include <iomanip.h>

extern "C" {
  int gettimeofday (struct timeval *, struct timezone *);
  int getrusage (int who, struct rusage *rusage);
  int getpagesize (void);
}


#ifdef A_ZIPPY
typedef int Asec_t;
typedef int Ausec_t;
#endif

#ifdef A_DEC_STATION
typedef long int Asec_t;
typedef long int Ausec_t;
#endif

#ifdef A_SPARC
typedef long int Asec_t;
typedef long int Ausec_t;
#endif

#ifdef A_HP
typedef unsigned long int Asec_t;
typedef long int Ausec_t;
#endif

class Arusage_t {
private:
  struct rusage before, after;
  struct rusage diff;		// temporary. stores the difference.
protected:
  void compute_difference (void);
public:
  Arusage_t (void);
  ~Arusage_t (void) {}
  void restart (void);
  void freeze (void);
  void print (ostream &os) const;
};

inline void Arusage_t::restart (void)
{
  int errval = getrusage (RUSAGE_SELF, &before);
  assert (errval >= 0);
}

inline Arusage_t::Arusage_t (void) 
{
  restart ();
}

inline void Arusage_t::freeze (void)
{
  int errval = getrusage (RUSAGE_SELF, &after);
  assert (errval >= 0);
  compute_difference ();
}

inline void Arusage_t::compute_difference (void) 
{
  diff.ru_utime.tv_sec = after.ru_utime.tv_sec - before.ru_utime.tv_sec;
  diff.ru_utime.tv_usec = after.ru_utime.tv_usec - before.ru_utime.tv_usec;
  diff.ru_stime.tv_sec = after.ru_stime.tv_sec - before.ru_stime.tv_sec;
  diff.ru_stime.tv_usec = after.ru_stime.tv_usec - before.ru_stime.tv_usec;
  diff.ru_minflt = after.ru_minflt - before.ru_minflt;
  diff.ru_majflt = after.ru_majflt - before.ru_majflt;
}

const double Akb_per_page = ((double)getpagesize ())/((double) 1024);

inline void Arusage_t::print (ostream &os) const
{
  double user_time = 
    (double)diff.ru_utime.tv_sec+(double)diff.ru_utime.tv_usec/1e6;
  double sys_time = 
    (double)diff.ru_stime.tv_sec+(double)diff.ru_stime.tv_usec/1e6;
  double total_time = user_time + sys_time;

  int maxrss = (int) anint ((double)after.ru_maxrss*Akb_per_page);

  os << "User time (secs.)  : " << user_time << endl;
  os << "System time (secs.): " << sys_time << endl;
  os << "Total time (secs.) : " << total_time << endl;
//  os << "Max. RSS (kB)      : " << maxrss << endl;
}

inline ostream &operator<< (ostream &os, const Arusage_t &rusage)
{
  rusage.print (os); return os;
}

class Atimer_t {
 private:
  struct timeval start_time;

 public:
  Atimer_t (void);
  ~Atimer_t (void) {}
  void restart (void);
  void print (ostream &os) const;
  void time (struct timeval *t) const;
  void time (struct timeval &t) const;
  void time (Asec_t &diff_sec, Ausec_t &diff_usec) const;
};


inline void Atimer_t::restart (void)
{
  struct timezone tz;
  tz.tz_minuteswest = tz.tz_dsttime = 0;
  assert (gettimeofday (&start_time, &tz) == 0);
}

inline Atimer_t::Atimer_t (void) 
{
  restart ();
}

inline void Atimer_t::time (Asec_t &diff_sec, Ausec_t &diff_usec) const
{
  struct timezone tz;
  tz.tz_minuteswest = tz.tz_dsttime = 0;
  struct timeval cur_time;
  assert (gettimeofday (&cur_time, &tz) == 0);
  diff_sec = cur_time.tv_sec - start_time.tv_sec;
  diff_usec = cur_time.tv_usec - start_time.tv_usec;
  if (diff_usec < 0)
    {
      assert (diff_sec > 0);
      diff_sec--; diff_usec += 1000000;
    }
  else if (diff_usec > 1000000)
    {
      diff_sec++; diff_usec -= 1000000;
    }
}

inline void Atimer_t::time (struct timeval *t) const
{
  time (t->tv_sec, t->tv_usec);
}

inline void Atimer_t::time (struct timeval &t) const
{
  time (t.tv_sec, t.tv_usec);
}
  

inline ostream &operator<< (ostream &os, const struct timeval &tv) 
{
  os << setw (os.width ()-7) << setfill (' ') << tv.tv_sec 
     << '.' <<  setw (6) << setfill ('0') << tv.tv_usec << setfill (' ');
  return os;
}

inline void Atimer_t::print (ostream &os) const
{
  struct timeval tv;
  time (tv);
  os << tv;
}

inline ostream &operator<< (ostream &os, const Atimer_t &timer)
{
  timer.print (os); return os;
}

#endif /* ATIMER_H */

#ifndef JULIANDAY
#define JULIANDAY

#include "config.h"

  // Standard C++ libraries

#include <cstdint>
#include <ctime>

namespace ACL
{
  FP_t const MJD0 = 2400000.5;      ///< Offset for calculating modified julian days

  class CAstronomicalCoordinates;

  /// @brief The TJD class is used as a representation for a Julian Day.
  /// @details The class allows storing and manipulating a Julian day value. The integer part of the JD is the Julian Days.
  ///          The fractional part stores the time since the start of the previous Julian day.

  class TJD		// Julian day/date class
  {
  private:
    FP_t JD_[2];

  protected:
    void normalise();

  public:
    TJD();																		    // Default constructor
    TJD(FP_t);                                    // Constructor taking a double.
    TJD(FP_t, FP_t);                              // Constructor taking two doubles.
    TJD(int, int, int);														// yy mm dd constructor
    TJD(unsigned int year, unsigned int month, unsigned int day, unsigned int hour, unsigned int minute, FP_t seconds);
    TJD(const TJD &);															// Copy Constructor
    TJD(struct tm *);
    TJD(time_t const &);

    TJD operator+(const TJD &) const;
    TJD operator-(const TJD &) const;
    TJD operator+(FP_t) const;
    TJD operator-(FP_t) const;
    TJD &operator +=(FP_t);
    TJD &operator-=(FP_t);
    TJD operator++(void);
    TJD operator++(int);
    TJD operator/(FP_t) const;
    TJD &operator=(FP_t);                   ///< Sets the JD to the double value passed.
    TJD &operator=(TJD const &);
    FP_t &operator()(int) ;                 ///< param = {0, 1} Specifies which double to get.
    FP_t const &operator()(int) const ;
    operator double() const;
    operator std::uint64_t() const;

    void JD(unsigned int nY, unsigned int nM, unsigned int nD);
    void JD(std::time_t const &);
    FP_t JD() const noexcept;

    FP_t JD0();
    unsigned long MJD() const;
    FP_t floor() const;
    FP_t HJD(CAstronomicalCoordinates const &);								// returns the heliocentric correction for the JD.
    const char *JDH(char *, int);

    size_t JD(char *, size_t, int = 1);
    bool gregorianDate(struct tm *) const;     // Converts the JD to a struct tm date.
    FP_t Epoch() const;
    std::pair<std::uint32_t, std::uint32_t> decompose();

    std::string gregorianDate() const;
    std::string timeOfDay();
  };

} // namespace ACL

#endif // JULIANDAY


#ifndef GAMMAVARIATE_H
#define GAMMAVARIATE_H



#include <set>
#include <boost/shared_ptr.hpp>

namespace itk {
class GammaCostFunction;
}

namespace GammaFunctions {



class GammaVariate {
  public:
    typedef std::pair< double, double > Sample;
    typedef std::set< Sample > SampleSetType;
    typedef SampleSetType::const_iterator SampleIt;
    typedef boost::shared_ptr< const SampleSetType > SampleSetConstPtrType;
    GammaVariate():nonConstSamples(new SampleSetType), samples(nonConstSamples) {};
    void setSamples(SampleSetConstPtrType sset) { samples = sset; nonConstSamples.reset();}
    void addSample(double t, double y);
    void clearSamples();
    double computeY( double t ) const;
    double getMaxSlope( void ) const;
    double getBaseline( void ) const;
    double getCenterOfGravity() const;
    double getAUC() const;
    double getMaximum( void ) const;
    
    void findFromSamples();
    void getParameters( double &t0_, double &tmax_, double &y0_, double &ymax_, double &alpha_ ) {
      t0_ = t0; tmax_ = t0max; y0_ = y0; ymax_ = y0max; alpha_ = alpha;
    }
    void setParameters( double t0_, double tmax_, double y0_, double ymax_, double alpha_ );

    double findAlpha();

  protected:
    double distanceToSamples() const;
    friend class itk::GammaCostFunction;

  private:
    void optimize();
    double t0;
    double t0max;
    double y0;
    double y0max;
    double alpha;
    boost::shared_ptr< SampleSetType > nonConstSamples;
    SampleSetConstPtrType samples;
};

}
#endif //GAMMAVARIATE_H
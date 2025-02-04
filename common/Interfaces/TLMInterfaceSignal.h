//!
//! \file TLMInterfaceSignal.h
//!
//! Provides a definition for the TLMInterfaceSignal class
//!
//!
//! \author   Robert Braun
//!

#ifndef TLMINTERFACESIGNAL_H
#define TLMINTERFACESIGNAL_H

#include "Interfaces/TLMInterface.h"

//!
//! TLMInterfaceSignal provides the base class for client side functionality for a single signal interface
//!
class TLMInterfaceSignal : public omtlm_TLMInterface {
public:
  TLMInterfaceSignal(TLMClientComm &theComm, std::string &aName, double StartTime, int Dimensions,
                     std::string Causality, std::string Domain="signal");

  //! Destructor. Sends the rest of the data if necessary.
  virtual ~TLMInterfaceSignal();

  //!  TimeData is the deque of data received from the coupled simulation.
  //!  The data is "pushed back" when received and "poped front" when the
  //!  time goes forward more than  TLM delay and old data is not needed any longer.
  std::deque<TLMTimeDataSignal> TimeData;

  //!  DataToSend stores the motion data from the interface. The data is sent
  //! in packet for a time period of [half] TLM delay [depends on solver type]
  std::vector<TLMTimeDataSignal> DataToSend;

  double InitialValue = 0;

  void GetTimeData(TLMTimeDataSignal &Instance);
  void GetTimeData(TLMTimeDataSignal &Instance, std::deque<TLMTimeDataSignal> &Data);
  void UnpackTimeData(TLMMessage &mess);
  void SendAllData();
  void SetInitialValue(double value);

  // Remove the data that is not needed (Simulation time moved forward)
  // We leave two time points intact, so that interpolation work
  static void clean_time_queue(std::deque<TLMTimeDataSignal> &Data, double CleanTime);

  //! linear_interpolate is called with a vector containing 2 points
  //! computes the interpolation (or extrapolation) point with the the linear
  //! interpolation (extrapolation) The points are submitted using the p0 & p1
  //!  The desired time is given by the Instance.time. Results are stored in Instance.
  //! If OnleForce is set, then the position and velocity are not computed.
  static void linear_interpolate(TLMTimeDataSignal& Instance, TLMTimeDataSignal& p0, TLMTimeDataSignal& p1);


  //! hermite_interpolate is called with a vector containing 4 points
  //! computes the interpolation point with the the polynomial that
  //! interpolates point 2 and 3 and have the derivative in these points
  //! equal to the center difference approximation at these points.
  //! The points are submitted using the iterator 'it' giving the
  //! first point in the sequence. The desired time is given
  //! by the Instance.time. Results are stored in Instance.
  //! If OnleForce is set, then the position and velocity are not computed.
  static void InterpolateHermite(TLMTimeDataSignal& Instance, std::deque<TLMTimeDataSignal>::iterator& it);
};

#endif // TLMINTERFACESIGNAL_H

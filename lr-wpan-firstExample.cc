/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2011 The Boeing Company
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author:  Tom Henderson <thomas.r.henderson@boeing.com>
 */

/*
 * Try to send data end-to-end through a LrWpanMac <-> LrWpanPhy <->
 * SpectrumChannel <-> LrWpanPhy <-> LrWpanMac chain
 *
 * Trace Phy state changes, and Mac DataIndication and DataConfirm events
 * to stdout
 */
#include <ns3/log.h>
#include <ns3/core-module.h>
#include <ns3/lr-wpan-module.h>
#include <ns3/propagation-loss-model.h>
#include <ns3/propagation-delay-model.h>
#include <ns3/simulator.h>
#include <ns3/single-model-spectrum-channel.h>
#include <ns3/constant-position-mobility-model.h>
#include <ns3/packet.h>

#include <iostream>

using namespace ns3;

static void DataIndication (McpsDataIndicationParams params, Ptr<Packet> p)
{
  NS_LOG_UNCOND ("Received packet of size " << p->GetSize ());
}

static void DataConfirm (McpsDataConfirmParams params)
{
  NS_LOG_UNCOND ("LrWpanMcpsDataConfirmStatus = " << params.m_status);
}

static void StateChangeNotification (std::string context, Time now, LrWpanPhyEnumeration oldState, LrWpanPhyEnumeration newState)
{
  NS_LOG_UNCOND (context << " state change at " << now.GetSeconds ()
                         << " from " << LrWpanHelper::LrWpanPhyEnumerationPrinter (oldState)
                         << " to " << LrWpanHelper::LrWpanPhyEnumerationPrinter (newState));
}

int main (int argc, char *argv[])
{
  CommandLine cmd;

  cmd.Parse (argc, argv);

  LrWpanHelper lrWpanHelper;

  // Enable calculation of FCS in the trailers. Only necessary when interacting with real devices or wireshark.
  // GlobalValue::Bind ("ChecksumEnabled", BooleanValue (true));

  // Create 9 nodes, and a NetDevice for each one
  Ptr<Node> n0 = CreateObject <Node> ();
  Ptr<Node> n1 = CreateObject <Node> ();
  Ptr<Node> n2 = CreateObject <Node> ();
  Ptr<Node> n3 = CreateObject <Node> ();
  Ptr<Node> n4 = CreateObject <Node> ();
  Ptr<Node> n5 = CreateObject <Node> ();
  Ptr<Node> n6 = CreateObject <Node> ();
  Ptr<Node> n7 = CreateObject <Node> ();
  Ptr<Node> n8 = CreateObject <Node> ();


  Ptr<LrWpanNetDevice> dev0 = CreateObject<LrWpanNetDevice> ();
  Ptr<LrWpanNetDevice> dev1 = CreateObject<LrWpanNetDevice> ();
  Ptr<LrWpanNetDevice> dev2 = CreateObject<LrWpanNetDevice> ();
  Ptr<LrWpanNetDevice> dev3 = CreateObject<LrWpanNetDevice> ();
  Ptr<LrWpanNetDevice> dev4 = CreateObject<LrWpanNetDevice> ();
  Ptr<LrWpanNetDevice> dev5 = CreateObject<LrWpanNetDevice> ();
  Ptr<LrWpanNetDevice> dev6 = CreateObject<LrWpanNetDevice> ();
  Ptr<LrWpanNetDevice> dev7 = CreateObject<LrWpanNetDevice> ();
  Ptr<LrWpanNetDevice> dev8 = CreateObject<LrWpanNetDevice> ();


  dev0->SetAddress (Mac16Address ("00:01"));
  dev1->SetAddress (Mac16Address ("00:02"));
  dev2->SetAddress (Mac16Address ("00:03"));
  dev3->SetAddress (Mac16Address ("00:04"));
  dev4->SetAddress (Mac16Address ("00:05"));
  dev5->SetAddress (Mac16Address ("00:06"));
  dev6->SetAddress (Mac16Address ("00:07"));
  dev7->SetAddress (Mac16Address ("00:08"));
  dev8->SetAddress (Mac16Address ("00:09"));

  //NS_LOG_UNCOND("adress:  " << dev0->GetAddress() << "Mac:  " << dev0->GetMac());
  // Each device must be attached to the same channel
  Ptr<SingleModelSpectrumChannel> channel = CreateObject<SingleModelSpectrumChannel> ();
  Ptr<LogDistancePropagationLossModel> propModel = CreateObject<LogDistancePropagationLossModel> ();
  Ptr<ConstantSpeedPropagationDelayModel> delayModel = CreateObject<ConstantSpeedPropagationDelayModel> ();
  channel->AddPropagationLossModel (propModel);
  channel->SetPropagationDelayModel (delayModel);

  dev0->SetChannel (channel);
  dev1->SetChannel (channel);
  dev2->SetChannel (channel);
  dev3->SetChannel (channel);  
  dev4->SetChannel (channel);
  dev5->SetChannel (channel);  
  dev6->SetChannel (channel);
  dev7->SetChannel (channel);
  dev8->SetChannel (channel);

  // To complete configuration, a LrWpanNetDevice must be added to a node
  n0->AddDevice (dev0);
  n1->AddDevice (dev1);
  n2->AddDevice (dev2);
  n3->AddDevice (dev3);
  n4->AddDevice (dev4);
  n5->AddDevice (dev5);
  n6->AddDevice (dev6);
  n7->AddDevice (dev7);
  n8->AddDevice (dev8);

  // Trace state changes in the phy
  dev0->GetPhy ()->TraceConnect ("TrxState", std::string ("phy0"), MakeCallback (&StateChangeNotification));
  dev1->GetPhy ()->TraceConnect ("TrxState", std::string ("phy1"), MakeCallback (&StateChangeNotification));
  dev2->GetPhy ()->TraceConnect ("TrxState", std::string ("phy2"), MakeCallback (&StateChangeNotification));
  dev3->GetPhy ()->TraceConnect ("TrxState", std::string ("phy3"), MakeCallback (&StateChangeNotification));
  dev4->GetPhy ()->TraceConnect ("TrxState", std::string ("phy4"), MakeCallback (&StateChangeNotification));
  dev5->GetPhy ()->TraceConnect ("TrxState", std::string ("phy5"), MakeCallback (&StateChangeNotification));
  dev6->GetPhy ()->TraceConnect ("TrxState", std::string ("phy6"), MakeCallback (&StateChangeNotification));
  dev7->GetPhy ()->TraceConnect ("TrxState", std::string ("phy7"), MakeCallback (&StateChangeNotification));
  dev8->GetPhy ()->TraceConnect ("TrxState", std::string ("phy8"), MakeCallback (&StateChangeNotification));

  Ptr<ConstantPositionMobilityModel> sender0Mobility = CreateObject<ConstantPositionMobilityModel> ();
  sender0Mobility->SetPosition (Vector (50,50,0));
  dev0->GetPhy ()->SetMobility (sender0Mobility);
  Ptr<ConstantPositionMobilityModel> sender1Mobility = CreateObject<ConstantPositionMobilityModel> ();
  sender1Mobility->SetPosition (Vector (50,0,0));
  dev1->GetPhy ()->SetMobility (sender1Mobility);
  Ptr<ConstantPositionMobilityModel> sender2Mobility = CreateObject<ConstantPositionMobilityModel> ();
  sender2Mobility->SetPosition (Vector (60,0,0));
  dev2->GetPhy ()->SetMobility (sender2Mobility);
  Ptr<ConstantPositionMobilityModel> sender3Mobility = CreateObject<ConstantPositionMobilityModel> ();
  sender3Mobility->SetPosition (Vector (100,50,0));
  dev3->GetPhy ()->SetMobility (sender3Mobility);
  Ptr<ConstantPositionMobilityModel> sender4Mobility = CreateObject<ConstantPositionMobilityModel> ();
  sender4Mobility->SetPosition (Vector (60,100,0));
  dev4->GetPhy ()->SetMobility (sender4Mobility);
  Ptr<ConstantPositionMobilityModel> sender5Mobility = CreateObject<ConstantPositionMobilityModel> ();
  sender5Mobility->SetPosition (Vector (50,100,0));
  dev5->GetPhy ()->SetMobility (sender5Mobility);
  Ptr<ConstantPositionMobilityModel> sender6Mobility = CreateObject<ConstantPositionMobilityModel> ();
  sender6Mobility->SetPosition (Vector (40,100,0));
  dev6->GetPhy ()->SetMobility (sender6Mobility);
  Ptr<ConstantPositionMobilityModel> sender7Mobility = CreateObject<ConstantPositionMobilityModel> ();
  sender7Mobility->SetPosition (Vector (0,50,0));
  dev7->GetPhy ()->SetMobility (sender7Mobility);
  Ptr<ConstantPositionMobilityModel> sender8Mobility = CreateObject<ConstantPositionMobilityModel> ();
  sender8Mobility->SetPosition (Vector (40,0,0));
  dev8->GetPhy ()->SetMobility (sender8Mobility);

  McpsDataConfirmCallback cb0;
  cb0 = MakeCallback (&DataConfirm);
  dev0->GetMac ()->SetMcpsDataConfirmCallback (cb0);

  McpsDataIndicationCallback cb1;
  cb1 = MakeCallback (&DataIndication);
  dev0->GetMac ()->SetMcpsDataIndicationCallback (cb1);

  McpsDataConfirmCallback cb2;
  cb2 = MakeCallback (&DataConfirm);
  dev1->GetMac ()->SetMcpsDataConfirmCallback (cb2);

  McpsDataIndicationCallback cb3;
  cb3 = MakeCallback (&DataIndication);
  dev1->GetMac ()->SetMcpsDataIndicationCallback (cb3);

  McpsDataConfirmCallback cb4;
  cb4 = MakeCallback (&DataConfirm);
  dev2->GetMac ()->SetMcpsDataConfirmCallback (cb4);

  McpsDataIndicationCallback cb5;
  cb5 = MakeCallback (&DataIndication);
  dev2->GetMac ()->SetMcpsDataIndicationCallback (cb5);

  McpsDataConfirmCallback cb6;
  cb6 = MakeCallback (&DataConfirm);
  dev3->GetMac ()->SetMcpsDataConfirmCallback (cb6);

  McpsDataIndicationCallback cb7;
  cb7 = MakeCallback (&DataIndication);
  dev3->GetMac ()->SetMcpsDataIndicationCallback (cb7);

  McpsDataConfirmCallback cb8;
  cb8 = MakeCallback (&DataConfirm);
  dev4->GetMac ()->SetMcpsDataConfirmCallback (cb8);

  McpsDataIndicationCallback cb9;
  cb9 = MakeCallback (&DataIndication);
  dev4->GetMac ()->SetMcpsDataIndicationCallback (cb9);

  McpsDataConfirmCallback cb10;
  cb10 = MakeCallback (&DataConfirm);
  dev5->GetMac ()->SetMcpsDataConfirmCallback (cb10);

  McpsDataIndicationCallback cb11;
  cb11 = MakeCallback (&DataIndication);
  dev5->GetMac ()->SetMcpsDataIndicationCallback (cb11);

  McpsDataConfirmCallback cb12;
  cb12 = MakeCallback (&DataConfirm);
  dev6->GetMac ()->SetMcpsDataConfirmCallback (cb12);

  McpsDataIndicationCallback cb13;
  cb13 = MakeCallback (&DataIndication);
  dev6->GetMac ()->SetMcpsDataIndicationCallback (cb13);

  McpsDataConfirmCallback cb14;
  cb14 = MakeCallback (&DataConfirm);
  dev7->GetMac ()->SetMcpsDataConfirmCallback (cb14);

  McpsDataIndicationCallback cb15;
  cb15 = MakeCallback (&DataIndication);
  dev7->GetMac ()->SetMcpsDataIndicationCallback (cb15);  

  McpsDataConfirmCallback cb16;
  cb16 = MakeCallback (&DataConfirm);
  dev8->GetMac ()->SetMcpsDataConfirmCallback (cb16);

  McpsDataIndicationCallback cb17;
  cb17 = MakeCallback (&DataIndication);
  dev8->GetMac ()->SetMcpsDataIndicationCallback (cb17);

  // Tracing
  lrWpanHelper.EnablePcapAll (std::string ("lr-wpan-firstExample"), true);
  AsciiTraceHelper ascii;
  Ptr<OutputStreamWrapper> stream = ascii.CreateFileStream ("lr-wpan-firstExample.tr");
  lrWpanHelper.EnableAsciiAll (stream);

  //50 byte'lık paketi 1. cihaz 2. cihaza gönderiyor, 2 saniye sonra 2.cihaz 60 byte'lık paketi 1. cihaza gönderiyor.
  Ptr<Packet> p0 = Create<Packet> (50);  // 50 bytes of dummy data
  McpsDataRequestParams params;
  params.m_dstPanId = 0;

  params.m_srcAddrMode = SHORT_ADDR;
  params.m_dstAddrMode = SHORT_ADDR;
  params.m_dstAddr = Mac16Address ("00:01");
  params.m_msduHandle = 0;
  params.m_txOptions = TX_OPTION_ACK;

  Ptr<Packet> p1 = Create<Packet> (60);  // 60 bytes of dummy data
 
  Simulator::ScheduleWithContext (1, Seconds (1.0),
                                  &LrWpanMac::McpsDataRequest,
                                  dev8->GetMac (), params, p0);

  Simulator::ScheduleWithContext (2, Seconds (2.0),
                                  &LrWpanMac::McpsDataRequest,
                                  dev1->GetMac (), params, p1);
 
  Simulator::ScheduleWithContext (3, Seconds (5.0),
                                  &LrWpanMac::McpsDataRequest,
                                  dev2->GetMac (), params, p0);

  Simulator::ScheduleWithContext (4, Seconds (7.0),
                                  &LrWpanMac::McpsDataRequest,
                                  dev3->GetMac (), params, p1);

  Simulator::ScheduleWithContext (5, Seconds (13.0),
                                  &LrWpanMac::McpsDataRequest,
                                  dev4->GetMac (), params, p0);
 
  Simulator::ScheduleWithContext (6, Seconds (18.0),
                                  &LrWpanMac::McpsDataRequest,
                                  dev5->GetMac (), params, p1);

  Simulator::ScheduleWithContext (7, Seconds (24.0),
                                  &LrWpanMac::McpsDataRequest,
                                  dev6->GetMac (), params, p0);

  Simulator::ScheduleWithContext (8, Seconds (35.0),
                                  &LrWpanMac::McpsDataRequest,
                                  dev7->GetMac (), params, p1);

  Simulator::Run ();

  Simulator::Destroy ();
  return 0;
}

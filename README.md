FNCS-Tutorial
=============
Welcome to the Framework for Network Co-Simulation (FNCS) Tutorial.
This tutorial features two step-by-step guides that target the basics
('small') and a fully-featured use case ('large').

The [small demo](demo-small) showcases the fundamentals of running
co-simulations using the FNCS framework. We will walk you through
installing FNCS and its one dependency, ZeroMQ, as well as how to run
two sample simulators within the co-simulation platform provided by
FNCS. The code samples provided will illustrate the principles of
co-simulation including message exchange and clock synchronization.

The [large demo](demo-large) will walk you through installing FNCS,
GridLAB-D, ns-3, MATPOWER, and all prerequisite software. This complex
example application demonstrates a real use case of transactive control,
exchanging market prices and bids through a simulated network. Multiple
GridLAB-D instances individually simulate a single feeder and market,
while the cumulative demand of each feeder is exchanged with a MATPOWER
optimal power flow solver at the tranmission level.

We hope you find these two demonstration cases useful. Please use the
GitHub Issues to notify us of any problems with the tutorial text.

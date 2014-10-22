FNCS-Tutorial
=============
Welcome to the Framework for Network Co-Simulation (FNCS) Tutorial.
This tutorial features three step-by-step guides that target the basics
('power+power', 'power+network') and a fully-featured use case ('large').

The [power+power demo](demo-power-power) showcases the
fundamentals of running co-simulations using the FNCS framework. We will
walk you through installing FNCS and its one dependency, ZeroMQ, as well
as how to run two sample simulators within the co-simulation platform
provided by FNCS. The code samples provided will illustrate the
principles of co-simulation including message exchange and clock
synchronization. Using two toy power simulators are representative of
what we call "tick-based" simulators, as opposed to event-based
simulators which are covered in the next demo.

The [power+network demo](demo-power-network) increases the complexity of
co-simulation by combining a power simulator with a network simulator.
Combining tick-based simulators with event-based simulators can be
frustrating, but we provide interfaces for each type of simulator.

The [large demo](demo-large) will walk you through installing FNCS,
GridLAB-D, ns-3, MATPOWER, and all prerequisite software. This complex
example application demonstrates a real use case of transactive control,
exchanging market prices and bids through a simulated network. Multiple
GridLAB-D instances individually simulate a single feeder and market,
while the cumulative demand of each feeder is exchanged with a MATPOWER
optimal power flow solver at the tranmission level.

We hope you find these two demonstration cases useful. Please use the
GitHub Issues to notify us of any problems with the tutorial text.

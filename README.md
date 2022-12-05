# Butterfly-gate

Arduino code and schmatics for a servo actuated butterfly gates.

System to direct vacuum extraction flow along trunk lines to specific section of a workshop.
Central control system controls which gate is open and which are closed so flow is directed to tool in operation.

Inputs to control the gates are from 3 sources:
1. Terminals to support wired connection at the end of the extraction point. This may support manual or "tool activated" switching.
2. Push buttons on the control enclosure.
3. MQTT control   


Repository includes a CAD folder with plans to plasma cut and fold brackets and supports for blast gate.

Rough order BOM:
Blast gate:
+ 6mm threaded rod - axis of gate
+ 0.8mm BMT galvanised sheet steel - gate
+ Building strap - Mounting for Hobby Servos 
+ Servo
+ Chocolate block terminals
+ Self taping and sealing roofing screws

Control Enclosure
+ NodeMCU
+ Chocolate block terminals



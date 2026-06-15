Compile and run:

g++ -std=c++14 Extra_Assignment.cpp -o fleet
./fleet

Part 1
Relationships:
Entity | Inheritance (IS-A) | class Spacecraft : public Entity | Spacecraft is a trackable entity; shares identity and interface
FuelTank | Composition (HAS-A) | FuelTank fuelTank; | Dies with the ship; no external owner; embedded by value
Telemetry | Composition (HAS-A) | Telemetry telemetry; | Dies with the ship; internal black-box; embedded by value
Fleet | Aggregation (HAS-A) | Fleet* fleet; | Ship can join or leave; Fleet outlives any single ship
Module | Aggregation (HAS-A) | Module* module; | Can be detached or swapped; lifecycle independent of the ship
Maneuver | Dependency (USES-A) | parameter in execute(const Maneuver&) | One-shot command; no persistent ownership needed

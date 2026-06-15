#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <stdexcept>
using namespace std;

class Entity{
    protected:
    string id;

    public:
    Entity(string i): id(i){
        cout<<"Entity creada"<<endl;
    }

    virtual ~Entity(){
        cout<<"Entity destruida"<<endl;
    }

    virtual void render() const = 0;
};

class Telemetry{
    private:
    string log;

    public:
    Telemetry(){
        cout<<"Telemetry creada"<<endl;
    }

    ~Telemetry(){
        cout<<"Telemetry destruida"<<endl;
    }

    void record(string msg){
        log += msg + "\n";
    }
};

class FuelTank{
    private:
    int fuel;

    public:
    FuelTank(int f = 100): fuel(f){
        cout << "FuelTank creado" << endl;
    }

    ~FuelTank(){
        cout << "FuelTank destruido" << endl;
    }

    int getFuel() const{ return fuel; }

    void consume(int amount){ fuel -= amount; }
};

class Module{
    protected:
    string name;
    int powerDraw;

    public:
    Module(string n, int p): name(n), powerDraw(p){
        cout<<"Module creado"<<endl;
    }

    // Without virtual, deleting through Module* skips the derived destructor:
    // Module* m = new Engine("X", 10);
    // delete m; Only ~Module() runs, ~Engine() is skipped,this leads to a emory leak
    // Fix: declare virtual ~Module() so the correct destructor is always called
    virtual ~Module(){
        cout<<"Module destruido"<<endl;
    }

    virtual void activate() const{
        cout<<"Modulo generico activado"<<endl;
    }
};

class Engine final : public Module{
    public:
    Engine(string n, int p): Module(n, p){
        cout<<"Engine creado"<<endl;
    }

    ~Engine(){
        cout<<"Engine destruido"<<endl;
    }

    void activate() const override{
        cout<<"Motores encendidos"<<endl;
    }
};

class Shield final : public Module{
    public:
    Shield(string n, int p): Module(n, p){
        cout<<"Shield creado"<<endl;
    }

    ~Shield(){
        cout<<"Shield destruido"<<endl;
    }

    void activate() const override{
        cout<<"Escudos levantados"<<endl;
    }
};

class Maneuver{
    private:
    string command;

    public:
    Maneuver(string c): command(c){}

    string getCommand() const{ return command; }
};

class CargoHold{
    private:
    int* cargoIds;
    size_t size;

    public:
    CargoHold(size_t s=0): cargoIds(new int[s]()), size(s){
        cout<<"CargoHold creado"<<endl;
    }

    ~CargoHold(){
        delete[] cargoIds;
    }

    CargoHold(const CargoHold& other): cargoIds(new int[other.size]), size(other.size){
        for(size_t i=0; i<size; i++) cargoIds[i]=other.cargoIds[i];
        cout<<"CargoHold copiado"<<endl;
    }

    CargoHold& operator=(const CargoHold& rhs){
        if(this==&rhs) return *this;
        delete[] cargoIds;
        size=rhs.size;
        cargoIds=new int[size];
        for(size_t i=0; i<size; i++) cargoIds[i]=rhs.cargoIds[i];
        return *this;
    }

    CargoHold(CargoHold&& other) noexcept: cargoIds(other.cargoIds), size(other.size){
        other.cargoIds=nullptr;
        other.size=0;
        cout<<"CargoHold movido"<<endl;
    }

    CargoHold& operator=(CargoHold&& rhs) noexcept{
        if(this!=&rhs){
            delete[] cargoIds;
            cargoIds=rhs.cargoIds;
            size=rhs.size;
            rhs.cargoIds=nullptr;
            rhs.size=0;
        }
        return *this;
    }

    void set(size_t i, int v){ if(i<size) cargoIds[i]=v; }
    //Aqui utilizo el operador ternario, una forma mas compacta de usar un if/else
    int get(size_t i) const{ return (i<size) ? cargoIds[i] : -1; }
    size_t getSize() const{ return size; }
};

class ModernCargoHold{
    private:
    vector<int> cargoIds;

    public:
    ModernCargoHold() = default;
    ModernCargoHold(vector<int> c): cargoIds(c){}
    void add(int id){ cargoIds.push_back(id); }
};

class DockingException: public runtime_error{
    public:
    DockingException(string msg): runtime_error(msg){}
};

// Forward declaration
class Spacecraft;

class Fleet{
    private:
    vector<unique_ptr<Spacecraft>> ships;

    public:
    void addShip(unique_ptr<Spacecraft> s){
        ships.push_back(move(s));
    }

    void removeShip(size_t index){
        if(index < ships.size())
            ships.erase(ships.begin() + index);
    }

    size_t count() const{ return ships.size(); }

    void showFleet() const;
};

class Spacecraft : public Entity{
    private:
    FuelTank fuelTank;
    Telemetry telemetry;
    Fleet* fleet;
    Module* module;

    public:
    Spacecraft(string id, Fleet* f)
        : Entity(id), fuelTank(), telemetry(), fleet(f), module(nullptr){
        cout<<"Spacecraft creada"<<endl;
    }

    ~Spacecraft(){
        cout<<"Spacecraft destruida"<<endl;
    }

    void render() const override{
        cout<<"Nave "<<id<<" en mapa"<<endl;
    }

    void mountModule(Module* m){ module=m; }

    void tick(){
        if(module) module->activate();
    }

    void execute(const Maneuver& m){
        cout<<"Ejecutando "<<m.getCommand()<<endl;
    }

    void dock(){
        if(fuelTank.getFuel()<50)
            throw DockingException("Combustible insuficiente");
        fuelTank.consume(10);
        cout<<"Dock exitoso"<<endl;
    }

    void consumeFuel(int amount){ fuelTank.consume(amount); }
};
//implementación de showFleet, recorre todo ships y llama render para cada uno jeje
//utilizo el Fleet::showfleet para decirle al compilador que este es parte de fleet, lo hago porque spacecraft aun no existe cuando declaro fleet, entonces sirve para despues llamarlo
void Fleet::showFleet() const{
    for(int i = 0; i < ships.size(); i++){
        ships[i]->render();
    }
}

int main(){
    Fleet fleet;

    fleet.addShip(make_unique<Spacecraft>("SC-01", &fleet));
    fleet.addShip(make_unique<Spacecraft>("SC-02", &fleet));

    fleet.showFleet();

    Engine engine("HyperDrive", 30);

    Spacecraft demo("SC-DEMO", &fleet);
    demo.mountModule(&engine);

    Maneuver m("Orbital Burn");
    demo.execute(m);
    demo.tick();

    // Dock exitoso
    try{
        demo.dock();
    }
    catch(const DockingException& e){
        cout<<e.what()<<endl;
    }

    // Dock fallido
    demo.consumeFuel(80);
    try{
        demo.dock();
    }
    catch(const DockingException& e){
        cout<<e.what()<<endl;
    }

    // Rule of 5
    CargoHold c1(3);
    c1.set(0,101); c1.set(1,202); c1.set(2,303);
    CargoHold c2(c1);
    c2.set(0,999);
    cout<<"Original[0]="<<c1.get(0)<<"  Copia[0]="<<c2.get(0)<<endl;
    CargoHold c3(move(c1));
    cout<<"Movido[0]="<<c3.get(0)<<"  Original.size="<<c1.getSize()<<endl;

    // Fleet sobrevive al destruir una nave
    cout<<"Naves: "<<fleet.count()<<endl;
    fleet.removeShip(0);
    cout<<"Naves tras eliminar SC-01: "<<fleet.count()<<endl;
    fleet.showFleet();

    return 0;
}
#ifndef PORT_H
#define PORT_H

class Wire;
class Inst;

class Port : public CircuitElement {
  public:
    Port(std::string name_) : CircuitElement(name_) {}
    
    void set_wire(Wire* wire_)
    {
        wire = wire_; 
    }
    
    void set_inst(Inst* inst_)
    {
        inst = inst_; 
    }
    
    Wire* get_wire()
    {
        return wire;
    }
    
    Inst* get_inst()
    {
        return inst;
    }
    
    CircuitElementType get_type() const
    {
        return PORT;   
    }
  
  private:
    Wire* wire;
    Inst* inst;
};


#endif

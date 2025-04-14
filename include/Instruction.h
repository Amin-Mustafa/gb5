#include <vector>
#include <string>
#include <map>

namespace GBA {

class Instruction{
private:
    using Args = std::vector<unsigned char>;
    using Opfn = void(*)(Args&);
    
    Opfn _op;
    Args _args;
    std::string _opname;
public:
    Instruction(Opfn op, const Args& args = {}, std::string opname = "")
        :_op{op}, _args{args}, _opname{opname} {}
    
    void print();
    void execute();
};

}
#include "TDKCommand.hpp"
#include "thalapathy/ir/IR.hpp"
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

namespace thalapathy {

class InspectCommand : public TDKCommand {
public:
    int execute(const TDKContext& ctx, const std::vector<std::string>& args) override {
        if (args.empty()) {
            std::cerr << "error: missing artifact file path (.vijay)\n";
            return 1;
        }

        std::string filepath = args[0];
        if (!fs::exists(filepath)) {
            std::cerr << "error: file not found: " << filepath << "\n";
            return 1;
        }

        ModuleIR ir;
        if (!ir.deserialize(filepath)) {
            std::cerr << "error: failed to deserialize VEAF artifact: " << filepath << "\n";
            return 1;
        }

        if (ctx.jsonOutput) {
            std::cout << "{\n"
                      << "  \"magic\": \"VJAY\",\n"
                      << "  \"formatVersion\": 3,\n"
                      << "  \"languageVersion\": \"0.3.0\",\n"
                      << "  \"constants\": " << ir.constantPool.size() << ",\n"
                      << "  \"instructions\": " << ir.instructions.size() << ",\n"
                      << "  \"classes\": " << ir.classes.size() << "\n"
                      << "}\n";
        } else {
            std::cout << "VEAF Artifact Summary:\n"
                      << "  Magic Signature: VJAY\n"
                      << "  Format Version: 3\n"
                      << "  Language Version: 0.3.0\n"
                      << "  Constants Pool Size: " << ir.constantPool.size() << " entries\n"
                      << "  Instructions Size: " << ir.instructions.size() << " instructions\n"
                      << "  Classes: " << ir.classes.size() << "\n"
                      << "  Interfaces: " << ir.interfaces.size() << "\n"
                      << "  Properties: " << ir.properties.size() << "\n"
                      << "  Methods: " << ir.methods.size() << "\n"
                      << "  Types: " << ir.types.size() << "\n";
        }

        return 0;
    }
};

} // namespace thalapathy

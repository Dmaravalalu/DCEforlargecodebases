//===- DeadFeaturePass.cpp - Phase 2 skeleton ------------------*- C++ -*-===//
//
// Out-of-tree LLVM module pass (new pass manager) that:
//   1. Loads the JSON oracle produced by the Python build extractor.
//   2. Walks every function and basic block in the module.
//   3. Prints each function name.
//
// Future phases will use the oracle to fold branch conditions tied to
// inactive build-time macros and mark whole basic blocks as dead.
//
// Run with:
//   opt -load-pass-plugin=./libDeadFeaturePass.so \
//       -passes=dead-feature \
//       -dead-feature-oracle=build_oracle.json \
//       -disable-output input.ll
//
//===----------------------------------------------------------------------===//

#include "llvm/IR/PassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/JSON.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

// CLI option that lets the user point the pass at the JSON oracle.
static cl::opt<std::string> OracleJsonPath(
    "dead-feature-oracle",
    cl::desc("Path to JSON file produced by extract_defines.py"),
    cl::value_desc("filename"),
    cl::init(""));

namespace {

struct DeadFeaturePass : public PassInfoMixin<DeadFeaturePass> {

  // Parse the oracle file and dump a short summary. Returns true on success.
  // For this skeleton we don't yet act on the contents.
  bool loadOracle(StringRef Path) {
    auto FileOrErr = MemoryBuffer::getFile(Path);
    if (auto Err = FileOrErr.getError()) {
      errs() << "[dead-feature] could not open oracle '" << Path
             << "': " << Err.message() << "\n";
      return false;
    }
    auto Parsed = json::parse((*FileOrErr)->getBuffer());
    if (!Parsed) {
      errs() << "[dead-feature] JSON parse error: "
             << toString(Parsed.takeError()) << "\n";
      return false;
    }
    const json::Object *Root = Parsed->getAsObject();
    if (!Root) {
      errs() << "[dead-feature] oracle root is not a JSON object\n";
      return false;
    }
    const json::Object *Defines = Root->getObject("defines");
    if (!Defines) {
      errs() << "[dead-feature] oracle has no 'defines' object\n";
      return false;
    }
    errs() << "[dead-feature] loaded oracle '" << Path << "' with "
           << Defines->size() << " macro(s)\n";
    return true;
  }

  PreservedAnalyses run(Module &M, ModuleAnalysisManager &) {
    errs() << "[dead-feature] analyzing module: " << M.getName() << "\n";

    if (!OracleJsonPath.empty())
      loadOracle(OracleJsonPath);
    else
      errs() << "[dead-feature] note: -dead-feature-oracle not provided\n";

    for (Function &F : M) {
      if (F.isDeclaration())
        continue;
      errs() << "  function: " << F.getName() << "\n";
      unsigned BBIdx = 0;
      for (BasicBlock &BB : F) {
        errs() << "    bb[" << BBIdx++ << "] "
               << (BB.hasName() ? BB.getName() : "<unnamed>")
               << "  (" << BB.size() << " inst)\n";
      }
    }

    // We didn't modify the IR.
    return PreservedAnalyses::all();
  }

  // Allow the pass to run even on functions marked optnone.
  static bool isRequired() { return true; }
};

} // end anonymous namespace

//===----------------------------------------------------------------------===//
// New pass manager plugin registration
//===----------------------------------------------------------------------===//

PassPluginLibraryInfo getDeadFeaturePassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "DeadFeaturePass", LLVM_VERSION_STRING,
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, ModulePassManager &MPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "dead-feature") {
                    MPM.addPass(DeadFeaturePass());
                    return true;
                  }
                  return false;
                });
          }};
}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return getDeadFeaturePassPluginInfo();
}

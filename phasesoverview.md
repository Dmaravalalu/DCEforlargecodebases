Phase 1: The "Build Oracle" (Configuration Extraction)

Standard dead code elimination doesn't know what you intended to build; it only knows the final code. You need a tool to capture that intent.

Action: Write a Python script to parse compile_commands.json (a standard file CMake generates).

Goal: Extract every -D flag (like -DENABLE_FEATURE_X=1). This gives your analyzer the "ground truth" of what is supposed to be active.

Phase 2: The Infrastructure (IR Generation)

You cannot analyze C++ source code directly with LLVM; you must analyze the Intermediate Representation (IR).

Action: Use Clang to compile your target project into .ll or .bc files.

Goal: Set up an "Out-of-Tree" LLVM Pass template. This is a standalone C++ project that links to LLVM libraries but lives outside the massive LLVM source tree, making it much faster to build and test.

Phase 3: The Analyzer (Logic & Reachability)

This is where you bridge the Build Flags with the Code.

Action: Your LLVM Pass will scan the IR for "Branch Instructions" (if statements).

Logic: If a branch depends on a constant that your "Build Oracle" says is never enabled, you mark that entire block of code as a Dead Feature.

Complexity: You will use Whole-Program Analysis (LTO - Link Time Optimization) to ensure you aren't just looking at one file, but the entire project.

Phase 4: The Accountant (Reporting & Volume)

Action: Use LLVM's debug metadata to map the "dead" IR blocks back to specific line numbers in the original .cpp files.

Goal: Calculate the total "removable" lines of code and the estimated reduction in the final binary size
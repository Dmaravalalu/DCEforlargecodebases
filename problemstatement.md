Assignment 29 - Dead Feature Detector for Large C/C++ Codebases Description: A whole-program LLVM analysis that identifies code regions guarded by preprocessor flags or runtime feature toggles that are unreachable under any actual build configuration or input — going beyond dead code elimination to detect dead features. Background: Large codebases accumulate feature flags (#ifdef, runtime config checks) where some combinations are never actually built or triggered. Traditional dead code elimination works at the single-TU level. No tool combines build-system-level configuration analysis (which #defines are actually used across all build targets) with IR-level reachability to identify entire feature-guarded code regions that are dead across all configurations. For example, LLVM itself has #ifdef-guarded code for experimental backends, deprecated features, and platform-specific paths — some of which may be unreachable in any current build configuration. Objective: Ingest build system data (CMake, Makefile) to enumerate actual build configurations, combine with IR-level reachability analysis, and report feature-guarded code blocks that are dead across all real configurations. Deliverables:

Build configuration extractor parsing CMake/Makefile for actual #define combinations
LLVM whole-program analysis correlating configuration predicates with IR reachability
Report of dead features with confidence scores and affected source regions
Evaluation on LLVM itself (or another large open-source project)
Estimate of removable code volume (lines, binary size savings)

Deliverable 1: Build Configuration Extractor
You need to write a tool that reads CMake or Makefile files. It must identify all the different #define combinations used in actual builds.
Example: If a feature #define EXPERIMENTAL_V3 is never set to "ON" in any of the CMake files, your tool should flag it as a candidate for being "dead."

Deliverable 2: LLVM Whole-Program Analysis
This is the technical core. You will write an LLVM Pass. This pass will:  
+1
Correlate the build-system flags (from Deliverable 1) with the code inside the LLVM IR.
Perform Reachability Analysis: Map out the "flow" of the program to see if those feature-guarded sections can ever be executed.  

Deliverable 3: The Report
A document or data file that lists the dead features found. It must include:
Confidence Scores: How sure are you that it's dead?
Affected Source Regions: Exactly which lines of C++ code can be deleted.

Deliverable 4: Evaluation & Estimation
You must test your tool on a real project (like LLVM itself) and calculate:
Removable Code Volume: How many thousands of lines of "clutter" can be removed.
Binary Size Savings: How much smaller the final .exe or binary becomes once the dead weight is gone.
For CS315 Project Bonus, the following was implemented:

|--------------------|-----------------------------------|---------------------------------------------|
| Material           | Implementations                   | Relevant Files                              |
|--------------------|-----------------------------------|---------------------------------------------|
| Assignment 2: SSE  | SSE improvements for:             | idk/projects/idk/math/vector_detail.h       |
|                    | - vec4 dot product                | idk/projects/idk/math/vector.inl            |
|                    | - mat4 * vec4 multiplication      | idk/projects/idk/math/matrix.inl            |
|--------------------|-----------------------------------|---------------------------------------------|
| C++ thread         | Job system using a lockfree queue | idk/projects/idk/parallel/ThreadPool.h      |
| Memory coherence   | - lockfree queue is not entirely  | idk/projects/idk/parallel/ThreadPool.cpp    |
|                    |   stable, fell back to lock-free  | idk/projects/idk/parallel/queue.h           |
|                    |   circular buffer                 | idk/projects/idk/parallel/circular_buffer.h |
|--------------------|-----------------------------------|---------------------------------------------|

Compilation:
* WARNING: Engine takes at least 10 minutes to compile on the computers in Tesla.
* For the engine to run, the testproject.sln must be compiled. Without it, the engine will crash without
    the necessary C# scripts.
* Select 40 Launcher as the project to compile.
  * The errors in the AssetCompiler and GameLauncher are irrelevant for the submission
* Target the testproj.sln once the engine is done.
* Run the project.

Comparison of performance once SSE was done is available in sse_speedup.png, in the same folder.
- Note: Associated level is not provided because it would otherwise be too large for submission with the game's art assets.
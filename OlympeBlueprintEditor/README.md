Olympe Blueprint Editor

This project is a dedicated editor for node-based blueprints for the Olympe Engine.

Dependencies:
- IM Nodes (https://github.com/Nelarius/imnodes) or IM Graph (fork) for node graph UI
- nlohmann/json or similar for JSON serialization
- The project should link against Olympe engine libraries for runtime integration

Instructions:
- Add the project to your Visual Studio solution if not automatically detected
- Configure include/library paths for IM Nodes and JSON library
- Implement editor UI in src and connect with the engine via public APIs

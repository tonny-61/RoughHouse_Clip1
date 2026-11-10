# RoughHouse — Clip 1 (Unreal Engine 5.8)

## Scope and status
This project implements the first visible-progress clip for Soho and Slick. It includes Epic's Manny and Quinn placeholders, unarmed movement and attack animations, a light hit reaction, automatic lock-on facing, target-relative circling, a shared fight camera, a lit prototype arena, and an automatic 25-second demonstration.

The project source and assets are assembled. Compilation is pending because the workstation is missing the Visual Studio C++ toolchain required by UE 5.8. The Microsoft installer is currently blocked by the protected `VSStandardCollectorService150` service.

## Requirements
- Unreal Engine **5.8.x** (the .uproject association is set to 5.8)
- Visual Studio 2022 with **Game development with C++**, Windows SDK, MSVC and Unreal tooling

## Open and test
1. Extract this folder to a writable local directory, e.g. `D:\WORK\RoughHouseClip1`.
2. Right-click `RoughHouseClip1.uproject` > Generate Visual Studio project files (if available), or open it directly in Unreal Editor and allow the C++ module to build.
3. If the editor requests a different installed engine association, select your **5.8.x** installation.
4. Open the project. The default Entry world uses `RHGameMode` and spawns the complete runtime arena automatically.
5. Press Play. The automatic 25-second Clip 1 pass is enabled by default.
6. For manual control, disable `bAutoDemo` on `RHArenaPawn`. Use **W/S** to advance or retreat, **A/D** to circle, and **Space** to punch.

## Known limits
- Manny and Quinn are recognizable development placeholders for Soho and Slick.
- The current attack and reaction clips are Epic template animations and should be replaced when the final character assets arrive.
- No hit sounds, effects, health UI, knockdown, or advanced combo system. Those belong to later milestones.
- The source has not yet been compiled or play-tested because the local MSVC toolchain is absent.
- The arena is created at runtime, so no `.umap` is required for this blocking clip.
- A gameplay video still needs to be recorded after compilation succeeds.

## Client acceptance checklist
- [ ] Project builds in Unreal 5.8.x on Windows
- [ ] Two fighters appear and face each other
- [ ] Soho circles Slick using A/D while locked on
- [ ] Camera tracks both fighters
- [ ] Punches cause reactions within range
- [x] Use licensed Epic placeholder characters and animations
- [ ] Record a real 20–30 second gameplay capture
- [ ] Provide asset source URLs, licenses, and project archive

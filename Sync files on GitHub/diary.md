# Project Logbook / Diary

## Date: 13 May 2026
**Author:** Albert Ma

### 👥 Team Organization & Task Allocation
- Held an initial team introduction session to familiarize everyone with each other's strengths and backgrounds.
- Successfully defined and allocated specific project roles and responsibilities across the team (e.g., Movement & UI, individual sensor modules, Project Management) to ensure a clear distribution of workload.

### �️ Hardware & Assembly Progress
- Assembled the basic physical chassis and form of the EEELunarRover.
- Successfully completed basic motor diagnostics and testing. Confirmed that the H-bridge motor driver and differential steering are functioning correctly.

### 💻 Software & Project Management
- Established the central GitHub repository for the team's version control and collaboration.
- Migrated project management files, schedules, and task trackers from Notion to the `Sync files on GitHub` workspace to ensure all code and documentation remain synchronized in one place.

### 🎮 Web UI & Movement Control (Major Milestone)
- Developed the complete C++ backend for WiFi remote control, including an automated 500ms "Watchdog" safety mechanism to prevent crashes if the network connection drops.
- Designed and implemented a responsive "Hold-to-drive" Web UI featuring a live command history log for debugging. Deployed a temporary UI prototype via GitHub Pages for easy mobile access *(Note: This webpage is currently a temporary prototype used primarily as a template for backend adaptation and testing)*.
- Discussed and confirmed the "Tank-style" (rotate on the spot) steering logic with the team.
- Created comprehensive Field Test Instructions (`WiFi_Testing_Instructions.md`) for the team to execute in the lab tomorrow.

### 📅 Next Steps
- **Next Team Meeting:** Scheduled for the afternoon of 15 May.
- **Agenda:** Finalize sensor implementation strategies, confirm the components to order, and proceed with integrating the frontend web UI with the backend C++ motor control logic.

---
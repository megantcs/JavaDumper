# JavaDumper

## Overview
JavaDumper is a professional instrumentation tool designed for extracting class information from active Java processes via JVMTI. The tool provides HTTP-based remote control interface for runtime class analysis and export capabilities.

## System Requirements
- Windows OS (7+)
- Java 8+ Runtime Environment  
- Target Java process with JVMTI support

## Installation & Usage

### Step 1: Acquire Binary
Download the latest JavaDumper.dll release from the distribution channel.

### Step 2: Process Injection
Inject the DLL into the target Java process using your preferred injection method.

### Step 3: Execute Dump Command
```bash
curl.exe --http0.9 -X POST http://localhost:2716 -d "/dci --header ?ux ?iof of=OUTPUT_FILE.txt"
```

Upon successful execution, the output file containing all class definitions will be generated in the process working directory.

## Build Instructions

### Prerequisites
- Microsoft Visual Studio 2019 or later
- Windows SDK
- C++ build tools

### Compilation Process
1. Clone repository
2. Open command prompt in project directory  
3. Execute build command:
```bash
msbuild .\JavaDumper.sln
```

## Project Status
The current implementation is a functional prototype that provides core reset capabilities. Future developments will focus on improved filtering, performance optimization, and enhanced JVM support. Current goals include creating a client for more convenient interaction with the server, as it is currently done through curl.

## Legal Notice
This tool is intended for educational purposes, debugging, and security research. Users are responsible for complying with applicable laws and software licenses.

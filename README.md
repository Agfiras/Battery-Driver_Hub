<!-- Improved compatibility of back to top link: See: https://github.com/othneildrew/Best-README-Template/pull/73 -->
<a name="ADOK BATTERY"></a>
<!--
*** Thanks for checking out the Best-README-Template. If you have a suggestion
*** that would make this better, please fork the repo and create a pull request
*** or simply open an issue with the tag "enhancement".
*** Don't forget to give the project a star!
*** Thanks again! Now go create something AMAZING! :D
-->



<!-- PROJECT SHIELDS -->
<!--
*** I'm using markdown "reference style" links for readability.
*** Reference links are enclosed in brackets [ ] instead of parentheses ( ).
*** See the bottom of this document for the declaration of the reference variables
*** for contributors-url, forks-url, etc. This is an optional, concise syntax you may use.
*** https://www.markdownguide.org/basic-syntax/#reference-style-links
-->
[![MIT License][license-shield]][license-url]
[![LinkedIn][linkedin-shield]][linkedin-url]



<!-- PROJECT LOGO -->
<br />
<div align="center">
  <a href="https://github.com/Agfiras/Battery-Driver_Hub/">
    <img src="./Logos/icon.png" alt="Logo" width="200" height="200">
  </a>
  <h3 align="center">ADOK BATTERY</h3> <br>
  <p align="center">
    A solution to get Real-time informations of the battery's Hub
    <br>
   and project it in Windows
    <br />
    <a href="https://github.com/Agfiras/Battery-Driver_Hub/blob/main/Logos/Screenshot.gif">View Demo ➡️</a>
  </p>
</div>


<!-- TABLE OF CONTENTS -->
<details>
  <summary><strong>Table of Contents</strong></summary>
  <ol>
    <li>
      <a href="#about-the-project">About The Project</a>
      <ul>
        <li><a href="#functionality">Functionality</a></li>
        <li><a href="#built-with">Built With</a></li>
      </ul>
    </li>
    <li>
      <a href="#how-to-test">How to test</a>
      <ul>
        <li><a href="#prerequisites">Prerequisites</a></li>
        <li><a href="#test-instruction">Test instruction</a></li>
        <li><a href="#installation">Installation</a></li>
      </ul>
    </li>
    <li><a href="#license">License</a></li>
    <li><a href="#acknowledgments">Acknowledgments</a></li>
  </ol>
</details>



<!-- ABOUT THE PROJECT -->
## About The Project 

<p> ADOK BATTERY is an executable designed to interact with the Simbat driver and extract information from Pic Through COM Express board using the Cgos library. </p>

<p>This program updates the parameters of the Simbat driver, including battery <strong>capacity</strong>, <strong>rate</strong>, and <strong>power state</strong> parameters. It ensures synchronization and accurate representation of the battery status within the Windows system.</p>


### Functionality

* The program is configured to Read battery information and swipe it with the parameters of the simbatt driver with a specific device instance path.
* Change the device instance path in the code if necessary.

* The address and command for reading battery information can be configured in the ReadBatInfo function, in BatteryConfig/main.cpp
  * The address (addr) is set to 0x26, which corresponds to the PIC address.
  * Commands "0D"and "12" are used to retrieve specific battery information:
    
    * "0D" retrieves the battery percentage remaining.
    * "12" retrieves the power state and the time to empty.
    <br>
    🟩 (time to empty to show only in console for verification, the remaining time of windows is done by calcualtion of the Rate parameter)
      
### Built With
This Project is developed mainly with two languages , the excutable with C++ and th driver with C.
* [![C][C.js]][C-url]
* [![C++][C++.js]][C++-url]
<br> </br>
Optimized for the Windows x64 Environment using the Microsoft Visual Studio Integrated Development Environment (IDE).
* ![Windows][Windows.js]
* [![VisualStudio][Visual Studio.js]][Visual Studio-url]
  

## How to test
It's recommended to **test in a Adok HUB** during development, since faulty drivers might crash or corrupt the computer and it will crash since the computer do not run with COM Express board. You can use the "checkpoint" feature to roll back the machine to a known good state in case of driver installations problems.

### Prerequisites
Prerequisites for the _host_ computer:
* Install [Visual Studio](https://visualstudio.microsoft.com/).
* Install [Windows Driver Kit (WDK)](https://learn.microsoft.com/en-us/windows-hardware/drivers/download-the-wdk).
* (optional) Install [WiX toolset](https://wixtoolset.org/) for MSI installer packaging.

Prerequisites for the _target_HUB:
* Disable Secure Boot in UEFI/BIOS (run [check](./check_secure_boot.ps1)to check).
* Enable test-signed drivers: [`bcdedit /set testsigning on`](https://learn.microsoft.com/en-us/windows-hardware/drivers/install/the-testsigning-boot-configuration-option)(run [Enable](Enable.ps1))
* Enable kernel debugging: `bcdedit /debug on` (optional)

### Test instruction
You can with the driver **make Windows believe that it’s being powered**.

Steps:
* Build solution in Visual Studio or download binaries from [releases](../../releases).
* Run `INSTALL.bat` with admin privileges to install the driver a simulated batterie.
* Run `BatteryConfig.exe`.
* Run `UNINSTALL.bat` with admin privileges to uninstall the driver and delete simulated batterie.


### Installation

Below is an example of how you can install and setup your HUB.

1. Run `INSTALL.bat` with admin privileges or run [Install Driver](./Install_Driver.ps1) to install the driver simulated batterie.
2. Run [mysetup.exe](./mysetup.exe).
3. Reboot.


<!-- LICENSE -->
## License

Distributed under the MIT License. See `LICENSE.txt` for more information.


<!-- ACKNOWLEDGMENTS -->
## Acknowledgments

I've included a few of my favorites to kick things off!

* [Writing Battery Miniclass Drivers](https://learn.microsoft.com/en-us/windows-hardware/drivers/battery/writing-battery-miniclass-drivers)
* [Cgos Documentation](https://github.com/Agfiras/Battery-Driver_Hub/blob/main/Logos/Cgos.pdf)



<!-- MARKDOWN LINKS & IMAGES -->
<!-- https://www.markdownguide.org/basic-syntax/#reference-style-links -->

[license-shield]: https://img.shields.io/github/license/othneildrew/Best-README-Template.svg?style=for-the-badge
[license-url]: https://www.linkedin.com/in/firas-ajengui
[linkedin-shield]: https://img.shields.io/badge/-LinkedIn-black.svg?style=for-the-badge&logo=linkedin&colorB=555
[linkedin-url]: [https://linkedin.com/in/othneildrew](https://www.linkedin.com/in/firas-ajengui)
[C.js]: https://img.shields.io/badge/C-00599C?style=for-the-badge&logo=c&logoColor=whit
[C-url]: https://learn.microsoft.com/en-us/cpp/c-language/?view=msvc-170
[C++.js]: https://img.shields.io/badge/C%2B%2B-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white
[C++-url]: https://isocpp.org/
[Windows.js]: https://img.shields.io/badge/Windows-0078D6?style=for-the-badge&logo=windows&logoColor=white
[Visual Studio.js]: https://img.shields.io/badge/Visual_Studio-5C2D91?style=for-the-badge&logo=visual%20studio&logoColor=white
[Visual Studio-url]: https://visualstudio.microsoft.com/fr/downloads/

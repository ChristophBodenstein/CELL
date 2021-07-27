# Controller performance analysis for CELL processors

This project implements several control algorithms and measures the calculation performance.
For details, check the original diploma thesis on [DB-Thueringen](https://www.db-thueringen.de/receive/dbt_mods_00009058) \(german\).

The original code was made for and tested on the CELL system simulator. I reworked it a bit and tested it on a PS3 with Fedora core 9 and Cell SDK 3.1.
The IDE is a modified eclipse from CELL SDK.
Basic questions this project aims to answer:
* Is the CELL fast enough to compute the control algorithms?
* Is it a good idea (faster) to compute the algorithms on SPUs or is the PPC unit fast enough?
* What`s better: manual vectorization (using intrinsics) or let the compiler optimize everything?

You may find the answer to these questions in the log files inside this codebase.

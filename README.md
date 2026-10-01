# FixItNow – Appliance Repair Service Management

A console application in C++17 that manages an appliance repair service: employees, the catalog of repairable appliances, repair requests, automatic assignment of requests to technicians, a tick-based repair simulation and CSV reports.

Built as a university Object-Oriented Programming project (2025).

## Features

- **Employees**: receptionists, technicians and supervisors.
  - Add employees, change their name, remove them (resignation), find one by CNP, list all of them.
  - Validation: Romanian CNP (check digit and a real birth date), names of 3–30 characters, a real hire date that is not in the future, at least 16 years old at the hire date.
  - Monthly salary: base salary, loyalty bonus, transport allowance, the technician's repair bonus and the supervisor's management bonus.
- **Appliance catalog**: fridges, televisions and washing machines.
  - Add or remove repairable brands and models.
  - List the appliances that could not be repaired, sorted by how many times they appeared in requests.
- **Repair requests**:
  - Read from CSV files, each with a unique timestamp.
  - The estimated duration and the price are computed from the appliance's age and the problem's complexity.
  - Requests for unknown models are rejected and recorded.
- **Automatic assignment**: each request goes to a technician who has the matching skill and fewer than 3 active requests. When several qualify, the technician with the least total work gets it. Otherwise the request waits in a FIFO queue.
- **Real-time simulation**: tick by tick, technicians receive, process and complete requests, and the waiting queue is drained as they become free.
- **CSV reports**:
  - top 3 salaries;
  - the technician with the longest repair;
  - pending requests grouped by type, brand and model;
  - full exports of employees and requests.

## Requirements

- A C++17 compiler (`g++` 8 or newer)
- `make`. On Windows with MinGW, use `mingw32-make`.

## Build and run

```bash
make            # build (only changed files are recompiled)
make run        # build, then run
make clean      # delete the executable and build/
make rebuild    # clean + build
```

On Windows with MinGW, replace `make` with `mingw32-make`.

Without make:

```bash
g++ -std=c++17 -I./include main.cpp src/*.cpp -o main.exe
./main.exe
```

> Run the program from the project root: the test files (`tests/`) and the report folder (`reports/`) are relative to it. `reports/` is created automatically if it is missing.

At startup the program does three things before opening the menu:

1. Fills the catalog with the default repairable models.
2. Loads `tests/employees_valid.csv` and `tests/requests_valid.csv`.
3. Checks that the service has at least 3 technicians, 1 receptionist and 1 supervisor.

## Menu

```
1. Employee Management     add / change name / remove / find by CNP / list
2. Appliance Management    add or remove a brand/model / show catalog / unrepairable appliances
3. Request Processing      register a request / real-time simulation / list requests
4. Reports                 top 3 salaries / longest repair / pending requests / CSV exports
5. Exit
```

## Test data

The `tests/` folder contains valid and invalid data for employees and requests:

| File | Content | Expected result |
|---|---|---|
| `employees_valid.csv` | 2 receptionists, 5 technicians (with different skills), 2 supervisors | 9 employees added, 0 errors |
| `employees_invalid.csv` | invalid CNPs (too short, letters), under-age employees, an impossible date (`32-13-2020`) | 0 employees added, 6 errors, one per line |
| `requests_valid.csv` | 11 requests for models in the catalog | all accepted and assigned to technicians |
| `requests_invalid.csv` | 8 requests for brands/models the service cannot repair | status `rejected`, listed under *Appliance Management → Display unrepairable appliances* |

Each invalid line is reported with its line number and cause, and the program continues with the remaining lines:

```
Read error: Invalid employee on line 2, cause: The entered CNP is not valid
Read error: Invalid employee on line 3, cause: The employee must be at least 16 years old at the hire date
Read error: Invalid employee on line 5, cause: The hire date is not valid (expected a real DD-MM-YYYY date)
```

### Test scenarios

`main.cpp` chooses which files are loaded at startup. To run another scenario, comment or uncomment the `readEmployeesFromCSV` / `readRequestsFromCSV` lines in `main.cpp`, then rebuild.

| Scenario | Files loaded | What it shows |
|---|---|---|
| 1. Normal workflow (default) | `employees_valid` + `requests_valid` | automatic assignment, load balancing, simulation, reports |
| 2. Employee validation | `employees_invalid` | CNP, age and date errors. The service stops before the menu: it has no employees. |
| 3. Unrepairable appliances | `employees_valid` + `requests_invalid` | rejected requests and the list of unrepairable appliances |
| 4. Mixed | all four files | invalid lines are reported while the valid ones are still processed |

## Reports

The **Reports** menu writes these files to `reports/`. The versions in the repository were generated from the valid test data after a 5-tick simulation.

| File | Content |
|---|---|
| `top_3_salaries.csv` | the 3 highest salaries (ties sorted by name) |
| `technician_max_duration.csv` | all data of the technician with the longest repair |
| `pending_requests.csv` | waiting requests grouped by type, brand and model |
| `employees.csv` | every employee with their current salary |
| `requests.csv` | every request with its status, technician and receptionist |

## Project structure

```
├── main.cpp            entry point: catalog setup, CSV loading, menu
├── include/            class headers (.h)
├── src/                class implementations (.cpp)
├── tests/              CSV test data
├── reports/            generated CSV reports
├── Makefile
└── Documentation.md    architecture and implementation details
```

Each class has its own `.h`/`.cpp` pair. See [Documentation.md](Documentation.md) for the class design, the formulas and how the simulation works.

# 🍝 Philosophers

> A multithreaded simulation of the classical Dining Philosophers problem, developed in C using POSIX threads and mutexes.  
> Built as part of the 42 School curriculum to demonstrate low-level concurrency management.

---

## 📚 About the Project

The **Dining Philosophers Problem** is a classic concurrency challenge. This project aims to simulate philosophers sitting at a table, alternating between **thinking**, **eating**, and **sleeping** while using a limited number of shared forks (resources) without causing **deadlocks**, **data races**, or **starvation**.

It emphasizes thread lifecycle management, mutex synchronization, and timing precision—all implemented manually in C without external libraries.

---

## 🧩 Core Concepts

- **Multithreading** with `pthread`
- **Mutex synchronization** with `pthread_mutex_t`
- **Race condition prevention**
- **Deadlock avoidance strategies**
- **Starvation prevention**
- **Precise time management** using `gettimeofday()` or `clock_gettime()`
- **Clean memory management**

---

## 🚀 Getting Started

### ✅ Prerequisites

- Unix-like system (Linux or macOS)
- `make`
- `cc` (or `gcc`) compiler
- Bash-compatible shell

---

### 📦 Clone & Compile

```bash
git clone https://github.com/falhaimouni/philo.git
cd philo
make

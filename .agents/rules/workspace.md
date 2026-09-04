# Workspace Rules: Local Betting Company Parimutuel Totalisator Engine

This workspace governs the modernization and translation of a local betting company's Parimutuel Totalisator (Tote) wagering engine from legacy C++ into modern Java 21 and Spring Boot 3 microservices.

All agents operating in this workspace must strictly adhere to the domain rules, sub-agent orchestration workflows, and Java 21 engineering standards defined below.

---

## 1. Local Betting Company Tote Domain Specifications

The totalisator engine processes live wagering pools, deduces statutory fees, and calculates parimutuel dividends for racing and sports events according to statutory pari-mutuel tote regulations of a local betting company.

### 1.1 Statutory Deduction (Commission / Takeout)
- **Takeout Rate**: 19% statutory deduction across all gross betting pools (`commission_rate = 0.19`).
- **Net Pool Calculation**:
  $$\text{Net Pool} = \text{Gross Pool} \times (1.0 - 0.19) = \text{Gross Pool} \times 0.81$$
- Deductions must occur before dividend division. Commission amounts must be accurately accounted for regulatory auditing.

### 1.2 Statutory Floor Breakage (10-Cent Breakage Rule)
- **Breakage Step**: All payouts and dividends per unit wager ($1.00 base unit) must be **rounded down to the nearest multiple of \$0.10** (`RoundingMode.FLOOR`).
- **Formula**:
  $$\text{Dividend} = \left\lfloor \frac{\text{Raw Dividend}}{0.10} \right\rfloor \times 0.10$$
- Breakage is non-negotiable: fractional cents below a 10-cent boundary are never rounded up (e.g., \$2.49 becomes \$2.40; \$3.10 remains \$3.10).
- Retained breakage must be accounted for and reported as statutory retention.
- **Minimum Payout**: If a calculated dividend after deduction and breakage falls below \$1.00 (the base unit bet), the operator statutory minimum floor is \$1.00 (or statutory refund/guarantee), unless an official statutory deficit applies.

### 1.3 Parimutuel Pool Allocations

#### Win Pool (Single Winner)
- Applied to the 1st place finisher of an event.
- **Dividend Rate Formula**:
  $$\text{Raw Win Dividend} = \frac{\text{Net Win Pool}}{\text{Total Wagers on 1st Place Runner}}$$
  $$\text{Payable Win Dividend} = \text{floor}_{0.10}(\text{Raw Win Dividend})$$

#### Place Pool (Top 3 Finishers)
- Under Statutory tote rules, Place wagering covers the **top 3 official finishers** (1st, 2nd, and 3rd).
- **Net Pool Apportionment**:
  - The Net Place Pool is divided into 3 equal shares (one share for each placing position):
    $$\text{Place Share}_i = \frac{\text{Net Place Pool}}{3} \quad (i \in \{1, 2, 3\})$$
  - For each placed runner $i$, the dividend per unit bet is:
    $$\text{Place Dividend}_i = \text{floor}_{0.10}\left(\frac{\text{Place Share}_i}{\text{Total Wagers on Runner } i}\right)$$

### 1.4 Dead-Heat Dividend Distribution Rules

When two or more competitors finish in an unbreakable tie ("dead-heat"), pool allocations must be adjusted:

#### Dead-Heat for Win
- If $N$ runners dead-heat for 1st place:
  - The Net Win Pool is divided equally into $N$ equal parts:
    $$\text{Win Pool Share}_k = \frac{\text{Net Win Pool}}{N} \quad (k = 1, \dots, N)$$
  - Dividend for dead-heating runner $k$:
    $$\text{Win Dividend}_k = \text{floor}_{0.10}\left(\frac{\text{Win Pool Share}_k}{\text{Total Wagers on Runner } k}\right)$$

#### Dead-Heat for Place
- **Dead-Heat for 1st (2 runners)**:
  - The two runners occupy 1st and 2nd place. Each gets $1/3$ of the Net Place Pool.
  - The 3rd place finisher receives the remaining $1/3$ share.
- **Dead-Heat for 2nd (2 runners)**:
  - The 1st place finisher receives $1/3$ of the Net Place Pool.
  - The two tied runners share the remaining two place allocations ($2/3$ of Net Place Pool), receiving $(2/3) / 2 = 1/3$ each.
- **Dead-Heat for 3rd ($M$ runners)**:
  - 1st and 2nd place finishers each receive their full $1/3$ share.
  - The single 3rd place share ($1/3$ of Net Place Pool) is divided equally among the $M$ tied runners:
    $$\text{Place Share}_k = \frac{\text{Net Place Pool} / 3}{M}$$
    $$\text{Place Dividend}_k = \text{floor}_{0.10}\left(\frac{\text{Place Share}_k}{\text{Total Wagers on Runner } k}\right)$$
- **Generalized Multi-Way Dead-Heat Rule**:
  - If $M$ runners tie across rank boundaries encompassing $K$ available prize shares:
    $$\text{Combined Pool Allocation} = K \times \left(\frac{\text{Net Place Pool}}{3}\right)$$
    $$\text{Share per Tied Runner} = \frac{\text{Combined Pool Allocation}}{M}$$

---

## 2. Sub-Agent Orchestration Directives

When tackling complex translation and modernization tasks, agents must employ disciplined sub-agent delegation.

### 2.1 When to Spin Up Sub-Agents
Proactively evaluate and spin up sub-agents for parallel engineering tasks:
1. **Parallel Reverse Engineering**: Concurrent analysis of separate legacy C++ headers, algorithms, calculation routines, and data structures.
2. **Parallel Test Generation**: Concurrently generating unit tests, statutory financial edge-case tests, parimutuel test datasets, and Spring MockMvc integration tests.
3. **Parallel Service Translation**: Translating independent components in parallel (e.g., ticket ingestion pipeline, concurrency accumulators, dividend calculator, REST controller, audit logging).

### 2.2 Sub-Agent Execution Rules
- **Explicit Scoping**: Give every sub-agent an isolated role and non-overlapping target files to avoid merge collisions.
- **Reference Ingestion**: Direct sub-agents to consult `.agents/skills/effective-java-core` and `.agents/skills/effective-java-concurrency`.

### 2.3 Mandatory Parent Verification (Zero-Trust Policy)
- **Do NOT Trust Self-Reports**: Sub-agents frequently report success prematurely, miss edge cases, fail compilation, or drop requirements.
- **Parent Verification Gate**: The parent agent **must inspect and verify all code** generated by sub-agents:
  1. Verify file existence and readability.
  2. Run compilation checks (`javac` or build tool compile).
  3. Execute automated test suites (`mvn test` / `gradle test`).
  4. Assert mathematical correctness (19% deduction, 10-cent breakage floor, dead-heat distributions).
- **Remediation**: If a sub-agent's work fails or is incomplete, the parent agent must immediately fix the defect directly or re-delegate with targeted instructions.

---

## 3. Java 21 / Spring Boot 3 Engineering Standards

All modern Java code must reside under the root package `com.example.tote` (e.g., `com.example.tote.domain`, `com.example.tote.service`, `com.example.tote.calculator`, `com.example.tote.controller`).

### 3.1 Strict Monetary Math (`BigDecimal` with `RoundingMode.FLOOR`)
- **Prohibition on Floats/Doubles**: Never use `float`, `double`, `Double`, or `Float` for money, pool balances, dividends, or stake calculations (Effective Java Core Item 48: *Avoid float and double if exact answers are required*).
- **Instantiation**: Always instantiate `BigDecimal` using string constructors or `BigDecimal.valueOf(long)`:
  ```java
  // CORRECT
  BigDecimal stake = BigDecimal.valueOf(100L); // 100 dollars
  BigDecimal commissionRate = new BigDecimal("0.19");
  
  // FORBIDDEN - precision loss
  BigDecimal bad = new BigDecimal(0.19);
  ```
- **Floor Breakage Implementation**:
  ```java
  public static final BigDecimal COMMISSION_RATE = new BigDecimal("0.19");
  public static final BigDecimal BREAKAGE_STEP = new BigDecimal("0.10");

  public static BigDecimal applyFloorBreakage(BigDecimal rawDividend) {
      return rawDividend.divide(BREAKAGE_STEP, 0, RoundingMode.FLOOR)
                        .multiply(BREAKAGE_STEP)
                        .setScale(2, RoundingMode.UNNECESSARY);
  }
  ```

### 3.2 Immutable Domain Records (Effective Java Core Item 15)
- All Value Objects, DTOs, Event payloads, calculation inputs, and calculation outputs must be declared as Java 21 `record` types in `com.example.tote.domain` (Effective Java Core Item 15: *Minimize mutability*).
- Ensure defensive copies of collection fields using `List.copyOf`, `Set.copyOf`, or `Map.copyOf`.
- Example:
  ```java
  package com.example.tote.domain;

  import java.math.BigDecimal;
  import java.time.Instant;
  import java.util.Objects;

  public record BetTicket(
      String ticketId,
      String poolId,
      String runnerId,
      BigDecimal stake,
      Instant placedAt
  ) {
      public BetTicket {
          Objects.requireNonNull(ticketId, "ticketId required");
          Objects.requireNonNull(stake, "stake required");
          if (stake.compareTo(BigDecimal.ZERO) <= 0) {
              throw new IllegalArgumentException("Stake must be positive");
          }
      }
  }
  ```

### 3.3 High-Throughput Thread-Safe Pool Accumulators (`effective-java-concurrency`)
- Wagering pools ingest tickets at high velocity concurrently.
- Use lock-free or low-contention accumulators:
  - Prefer `LongAdder` for tracking total stakes in integer cents to eliminate thread contention under heavy load.
  - Use `ConcurrentHashMap` with atomic operations (`compute`, `merge`, `putIfAbsent`) for mapping runners to pool totals.
  - Keep lock scope minimal if explicit locks (`ReentrantReadWriteLock` or `StampedLock`) are required.
  - Safe snapshotting: Create immutable snapshots of the pool before triggering dividend calculation runs.

### 3.4 Clean RESTful API Design & Spring Boot 3
- Follow modern RESTful standards:
  - `@RestController` with clear `@RequestMapping("/api/v1/pools")` under package `com.example.tote.controller`.
  - Jakarta Validation (`@Valid`, `@NotNull`, `@Positive`) on all incoming request records.
  - Return RFC 7807 `ProblemDetail` for standardized error handling via `@ControllerAdvice` or `@RestControllerAdvice`.
  - Clean separation of concerns: Controller -> Service / Engine -> Repository / Storage.

### 3.5 Comprehensive Testing with JUnit 5 & AssertJ
- Use JUnit 5 (`@Test`, `@ParameterizedTest`, `@CsvSource`, `@MethodSource`).
- Use AssertJ fluent assertions (`assertThat(...)`).
- Mandatory test scenarios for tote calculation:
  1. Standard Win pool calculation and breakage verification.
  2. Standard Place pool calculation across 3 finishers.
  3. 2-way and 3-way Win dead-heats.
  4. Dead-heats for 2nd and 3rd place in Place pools.
  5. Floor breakage boundaries (\$1.09 -> \$1.00; \$1.19 -> \$1.10).
  6. Minimum payout guarantees (\$1.00 floor per unit bet).
  7. High-concurrency ticket ingestion tests verifying thread-safety and zero stake loss.

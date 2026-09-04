id: betting-engine-cpp-to-java-antigravity
summary: Modernize a local betting company's 22-year-old C++ tote engine to Java 21 & Spring Boot 3 with Google Antigravity Spec-Driven Development
categories: AI Agents, Modernization, C++, Java, Spring Boot, Financial Systems
environments: Web
status: Published
feedback link: https://github.com/example/agy-code-translation-demo
authors: Google Cloud Customer Engineering
keywords: docType:Codelab,category:LegacyModernization,product:Antigravity,product:GoogleCloud,product:Gemini,language:Java,language:Cpp,framework:SpringBoot
layout: scrolling

# Modernizing a Local Betting Company's 22-Year-Old C++ Tote Engine to Java 21 with Antigravity

## Overview & Business Context
Duration: 0:05:00

A local betting company operates the national totalisator wagering system, processing hundreds of millions in parimutuel bets annually across local race fixtures and international partner feeds.

At the heart of this wagering infrastructure lies a **22-year-old C++98 Parimutuel Totalisator Engine** originally commissioned in 2004. While resilient, the engine has become a critical operational liability:
* **The $10M+ 3-Year Vendor Dilemma:** The betting company leadership, under **Customer Director of Gaming & Enterprise Technology** and **Customer CIO**, is currently evaluating a massive multi-year Commercial-Off-The-Shelf (COTS) Request for Proposal (RFP) from external gaming vendors (e.g. Aurora / SportsBook). The vendor RFP carries a prohibitive **\$10.5M price tag over 3 years**, rigid software licensing, and locks the enterprise into an overseas vendor roadmap.
* **The 6–9 Month QA Bottleneck:** Every minor regulatory rule change mandated by the **local gambling regulatory authority**—such as adjusting minimum dividend guarantees or altering race delay cutoffs—requires expensive vendor change requests (CRs) taking up to 9 months to test and deploy.
* **The Silent IEEE 754 Float Truncation Bug:** Decades of legacy C++ single-precision `float` math have harbored subtle truncation defects during Dead-Heat splits. Truncating quotients to 10 cents using raw integer casts shorts payouts by 10 cents per dollar on specific pools, forcing accounting teams to perform manual ledger reconciliations.

```
                           THE STRATEGIC CROSSROADS
                           
    Option A: The $10.5M Vendor RFP              Option B: Antigravity AI Modernization
    ───────────────────────────────              ──────────────────────────────────────
    ❌ $10.5M CAPEX/OPEX across 3 years           ✅ 82% Cost Reduction (~$1.8M total)
    ❌ 6-9 month wait for minor rule changes      ✅ Deploy updates in days/hours
    ❌ Proprietary black-box vendor lock-in       ✅ 100% In-house IP ownership
    ❌ Risky "Big Bang" cutover in 2029           ✅ Phased, continuous verification
```

### The Customer's Vision: In-House Agility with Antigravity
The customer's strategic goal is clear: **reclaim sovereign technical ownership of the betting company's core wagering intellectual property**. Instead of outsourcing core capability to an external consortium, the local betting company can leverage **Google Antigravity** to execute an automated, spec-driven code translation from legacy C++98 to modern, enterprise-grade **Java 21 and Spring Boot 3.3.3**.

In this hands-on workshop, you will step into the shoes of a local betting company senior engineer pairing with Antigravity. You will discover how Antigravity's autonomous agentic workflow systematically reverse-engineers legacy code, synthesizes a regression safety harness, executes multi-subagent migration governed by `$effective-java-core`, and diagnoses deep floating-point precision flaws that eluded developers for two decades.

---

## System Architecture: C++98 Legacy vs Java 21 Target
Duration: 0:07:00

The modernization replaces a monolithic, memory-unsafe C++98 command-line engine with a cloud-native, stateless, highly-observable microservice architecture built on Java 21, Spring Boot 3.3.3, and high-precision financial arithmetic (`BigDecimal`).

### Architectural Topology

```
+───────────────────────────────────────────────────────────────────────────────────+
│                           LEGACY C++98 ARCHITECTURE (2004)                         │
│                                                                                   │
│  +────────────────────+       +───────────────────+       +────────────────────+  │
│  │ Legacy Bet Slips   │ ----> │ C++98 Tote Engine │ ----> │ Console / Flat Log │  │
│  │ (Raw C-Structs)    │       │ (IEEE 754 Float)  │       │ (Truncated Cents)  │  │
│  +────────────────────+       +───────────────────+       +────────────────────+  │
│                                         │                                         │
│                                ❌ Single-precision float truncation                 │
│                                ❌ Manual memory management / buffer overruns       │
│                                ❌ Zero automated regression tests                  │
+───────────────────────────────────────────────────────────────────────────────────+
                                          │
                                          │  GOOGLE ANTIGRAVITY AGENTIC MIGRATION
                                          ▼
+───────────────────────────────────────────────────────────────────────────────────+
│                        TARGET JAVA 21 ARCHITECTURE (2026)                          │
│                                                                                   │
│  +─────────────────────────────────────────────────────────────────────────────+  │
│  │                       Spring Boot 3.3.3 REST Ingress Layer                  │  │
│  │            POST /api/tote/calculate-dividends   |   GET /api/tote/health     │  │
│  +──────────────────────────────────────┬──────────────────────────────────────+  │
│                                         │                                         │
│                                         ▼                                         │
│  +─────────────────────────────────────────────────────────────────────────────+  │
│  │                Immutable Domain Model (Java 21 Records)                     │  │
│  │  • Runner.java        • BetSlip.java        • DividendResult.java          │  │
│  │  • RacePool.java      • BetType.java (WIN, PLACE)                           │  │
│  │  (Enforces $effective-java-core: Immutability, Defensive Copying)           │  │
│  +──────────────────────────────────────┬──────────────────────────────────────+  │
│                                         │                                         │
│                                         ▼                                         │
│  +─────────────────────────────────────────────────────────────────────────────+  │
│  │           High-Precision Parimutuel Engine (ToteCalculationService)         │  │
│  │  • BigDecimal Internal Scale 4 (Half-Even Rounding)                         │  │
│  │  • Statutory 10-Cent Floor Breakage (Scale 1)                               │  │
│  │  • Zero-Loss Dead-Heat N-Way Pool Partitioning                              │  │
│  │  • Minimum Dividend Guarantee Enforcement ($1.00 Floor)                     │  │
│  +──────────────────────────────────────┬──────────────────────────────────────+  │
│                                         │                                         │
│                                         ▼                                         │
│  +─────────────────────────────────────────────────────────────────────────────+  │
│  │                 Comprehensive JUnit 5 & AssertJ Test Suite                  │  │
│  │  • WinPoolTests       • PlacePoolTests      • DeadHeatCentForCentTests      │  │
│  +─────────────────────────────────────────────────────────────────────────────+  │
+───────────────────────────────────────────────────────────────────────────────────+
```

### Architectural Trade-off Analysis

| Architectural Dimension | Legacy C++98 Engine | Target Java 21 Spring Boot 3 Engine | Antigravity Benefit |
| :--- | :--- | :--- | :--- |
| **Arithmetic Precision** | 32-bit `float` (IEEE 754, 24-bit mantissa) | Arbitrary-precision `BigDecimal` (Scale 4 internal, Scale 1 floor) | Eliminates rounding variances & ledger write-offs |
| **Data Integrity** | Mutable `struct` arrays, unchecked pointers | Immutable Java 21 `record` types with compact validation | Guarantees zero side-effects & thread-safety |
| **Dead-Heat Handling** | Truncates float quotient via `(int)(div * 10)` | Pro-rata pool division with exact floor breakage | 100% compliance with local statutory rules |
| **Integration Pattern** | Batch terminal binary / shared file I/O | Cloud-native Spring Web REST API (`/api/tote/*`) | Real-time commingling with international partner feeds |
| **Defensive Design** | Undocumented C-strings (`char[64]`) | Defensive unmodifiable lists (`List.copyOf`) | Conforms strictly to `$effective-java-core` |
| **Test Coverage** | None (ad-hoc manual printfs) | JUnit 5, AssertJ parameterized test harness | Continuous regression gating in CI/CD |

---

## The Antigravity Spec-Driven Development Workflow
Duration: 0:03:00

Antigravity abandons primitive "code completion" autocomplete in favor of **Spec-Driven Development (SDD)**. Rather than asking an AI to guess code line-by-line, developers orchestrate specialized agents across 5 structured SDLC phases:

```
┌─────────────────┐     ┌──────────────────┐     ┌──────────────────┐
│     Phase 1     │     │     Phase 2      │     │     Phase 3      │
│  Plan, Grill-Me │ ──> │   Golden Test    │ ──> │    Autonomous    │
│  & Plan Review  │     │  Harness Gen     │     │ Multi-Subagent   │
│ (/plan, /grill) │     │  (Regression)    │     │  Migration       │
└─────────────────┘     └──────────────────┘     └────────┬─────────┘
                                                          │
                        ┌──────────────────┐     ┌────────▼─────────┐
                        │     Phase 5      │     │     Phase 4      │
                        │ Root Cause Bug   │ <── │   Architecture   │
                        │ Debugging & Fix  │     │ & Security Audit │
                        │    (/debug)      │     │    (/review)     │
                        └──────────────────┘     └──────────────────┘
```

Each phase below contains:
1. **🤖 The Agentic Prompt**: The exact prompt to copy-paste into Antigravity.
2. **📄 Expected Reference Code**: The verified architectural artifact synthesized by the agent.
3. **🧪 Terminal Verification**: Executable shell commands to validate behavior immediately.

---

## Phase 1: Architecture Blueprint & Translation Planning (`/plan`), Review & Feedback
Duration: 0:10:00

Before generating code, professional software architects inspect legacy codebases to map domain entities, uncover implicit business rules, and design target interfaces.

In Antigravity, **Planning Mode** (`/plan`) reads the legacy C++ codebase in `legacy-cpp/`, cross-references corporate design rules, and authors a structured `implementation_plan.md` artifact. The architect then conducts Human-in-the-Loop (HITL) review and leaves binding review comments before authorizing execution.

### Step 1: Initialize Translation Architecture Planning (`/plan`)
Open the **Antigravity Chat Panel** and submit the following command:

```text
/plan Analyze the legacy C++98 totalisator engine in `legacy-cpp/` and formulate an enterprise migration blueprint to Java 21 and Spring Boot 3.3.3 in `modern-java/`.
Your plan must specify:
1. Domain entity mapping from C++ structs (`Runner`, `BetSlip`, `DividendResult`) to Java 21 immutable records adhering to `$effective-java-core`.
2. Financial arithmetic strategy: Replacing IEEE 754 single-precision float math with `BigDecimal` maintaining scale 4 internally and statutory scale 1 floor breakage (statutory 10-cent rule).
3. Dead-heat equity algorithm for Win and Place pools without C-style truncation.
4. Target package hierarchy under `com.example.tote`: `model/`, `service/`, `controller/`.
```

![Submitting /plan Prompt in Antigravity Chat](images/agy-plan-input.png)

### 📄 Expected Architectural Artifact: `implementation_plan.md`
Antigravity analyzes `legacy-cpp/include/tote_engine.h` and `src/tote_engine.cpp` and creates an execution plan:

![Implementation Plan Artifact Card](images/agy-implementation-plan-card.png)

```markdown
# Local Betting Company Parimutuel Engine Modernization Plan

## Target Stack
- Language: Java 21 (LTS)
- Framework: Spring Boot 3.3.3
- Build Tool: Maven 3.9+
- Testing: JUnit 5, AssertJ Core

## Domain Model (Immutable Records)
- `com.example.tote.model.BetType`: Enum { WIN, PLACE }
- `com.example.tote.model.Runner`: record(int runnerNumber, String horseName, int finishPosition)
- `com.example.tote.model.BetSlip`: record(String ticketId, int raceId, BetType betType, int runnerNumber, BigDecimal stakeAmount)
- `com.example.tote.model.DividendResult`: record(int runnerNumber, String horseName, BetType betType, BigDecimal grossDividend, BigDecimal brokenDividend, boolean deadHeat, int deadHeatCount, BigDecimal totalPayout)
- `com.example.tote.model.RacePool`: record(int raceId, String venue, int raceNumber, BetType betType, BigDecimal takeoutRate, List<Runner> runners, List<BetSlip> bets)

## Core Calculation Rules
- Intermediate Scale: 4 decimal places with RoundingMode.HALF_EVEN
- Statutory Breakage: Scale 1 with RoundingMode.FLOOR (Regulatory Authority 10-cent rule)
- Statutory Minimum Payout: $1.00 per unit bet
- Dead-Heat Division: Equal partition of net pool among N tied finishers
```

### Step 2: Source Code Inspection Points (What to Verify Before Approving)
Before giving sign-off, verify that the implementation plan addresses the specific risks uncovered in the legacy C++ codebase:

![Side-by-Side Implementation Plan Review](images/agy-implementation-plan-view.png)

1. **Dead-Heat Dividend Truncation (`legacy-cpp/src/tote_engine.cpp:260-310`):**
   * *Legacy Flaw:* Float division `4100.0f / 1000.0f = 4.099999f` multiplied by `10.0f` and cast to `int` yielded `40`, producing `$4.00` instead of `$4.10`.
   * *Plan Verification:* Ensure the plan explicitly specifies `BigDecimal` division with `HALF_EVEN` at scale 4, followed by statutory scale 1 floor breakage.
2. **Statutory 10-Cent Floor Breakage (`legacy-cpp/src/tote_engine.cpp:20-24`):**
   * *Legacy Flaw:* Raw integer division `((int)(raw_dividend * 10.0f)) / 10.0f` caused rounding drift on edge amounts ($3.4999 -> $3.40).
   * *Plan Verification:* Confirm `applyBreakage` uses `BigDecimal.setScale(1, RoundingMode.FLOOR)` and enforces the statutory `$1.00` floor.
3. **Domain Immutability & Defensive Copying (`legacy-cpp/include/tote_engine.h`):**
   * *Legacy Flaw:* Mutable structs with raw arrays and pointer iteration.
   * *Plan Verification:* Confirm mapping to Java 21 `record` classes with defensive copies (`List.copyOf`) conforming to `$effective-java-core`.
4. **Stateless Service Architecture:**
   * Confirm `ToteCalculationService` is specified as completely stateless, making it thread-safe for Virtual Thread execution.

### Step 3: Click "Review", Add Review Comment & Click "Proceed" (HITL Sign-off)
Antigravity enforces strict **Human-in-the-Loop Governance**—the agent halts execution and cannot begin generating production code until you formally review the plan and approve it.

Look at the prompt displayed by Antigravity directly in the chat:
> *"Please review the implementation plan and click Proceed (or provide your review comments) to begin execution."*

![Human-in-the-Loop Review Comment and Proceed](images/agy-review-comment.png)

1. Click the **`Review ▾`** button in the upper right header of the `implementation_plan.md` editor tab (or click into the chat prompt input box).
2. Enter your binding engineering criteria:
```text
Approved with conditions:

1. Strictly enforce $effective-java-core: immutable records only, zero float/double primitives.
2. Ensure domain models use defensive copying (List.copyOf) for all collection fields.
3. Ensure ToteCalculationService is completely stateless for thread safety.
```
3. Click the prominent blue **`Proceed`** button on the `Implementation Plan` card to lock in the translation architecture.

### 🧪 Terminal Verification
Verify that the legacy C++ codebase is present and review its structure:
```bash
ls -la legacy-cpp/include/ legacy-cpp/src/
```

---

## Phase 2: Missing Test Safety Net & Interactive Test Hardening (`/grill-me`)
Duration: 0:10:00

A critical failure mode in legacy migrations is translating code *before* having automated tests, leading to undetected behavioral regressions.

In the legacy 2004 C++ codebase, **there were zero automated unit tests**. Testing was either performed manually or discovered through ledger discrepancies in production.

Instead of an engineer spending weeks guessing edge cases, Antigravity uses `/grill-me` to proactively interrogate the architect and establish the **Golden Baseline Regression Test Suite** first.

### Step 1: Trigger Interactive Test Hardening (`/grill-me`)
Submit the following prompt in Antigravity:

```text
/grill-me The legacy C++ codebase had zero automated unit tests. Interrogate me to design a comprehensive golden regression test harness for modern-java. Challenge me on the betting company's financial rules, 3-place pools, statutory 10-cent breakage floors, and dead-heat edge cases so we don't miss any historical flaws.
```

### Step 2: Interactive Regulatory & Edge Case Questions
Antigravity conducts an interactive interview directly in the chat with multiple-choice questions to resolve:
1. **Single Winner Win Pool:** $10,000 gross pool, 18% takeout, winning runner with $2,500 stake -> Assert raw dividend $3.2800, broken dividend $3.20, total payout $8,000.00.
2. **3-Place Place Pool:** $15,000 gross pool, 20% takeout ($12,000 net pool), divided equally across 3 places ($4,000 each) with 10-cent floor breakage.
3. **Breakage Edge Cases:** $3.4999 -> $3.40, $5.1900 -> $5.10, and minimum statutory guarantee ($0.92 -> $1.00).
4. **The Dead-Heat Flaw Test Case:** 2-way dead heat in a $10,000 pool (18% takeout, $4,100 per dead-heat share). Runner with $1,000 stake must yield EXACTLY $4.10 dividend and $4,100.00 payout, proving zero float truncation drift.

### Step 3: AI Automated Unit Testing (Golden Baseline Harness)
Antigravity takes the verified criteria and generates the complete JUnit 5 and AssertJ test suite in `modern-java/src/test/java/com/example/tote/service/ToteCalculationServiceTest.java`.

### 📄 Expected Reference Code: `ToteCalculationServiceTest.java`
```java
@Test
@DisplayName("Dead-Heat: Must produce exact $4.10 dividend and $4,100.00 payout without float truncation")
void shouldCalculateDeadHeatWithStatutoryCentForCentPrecision() {
    List<Runner> runners = List.of(
            new Runner(1, "Lim's Kosciuszko", 1), // Dead heat 1st
            new Runner(2, "Golden Monkey", 1),    // Dead heat 1st
            new Runner(3, "Street of Dreams", 3)
    );

    List<BetSlip> bets = List.of(
            new BetSlip("DH-001", 103, BetType.WIN, 1, new BigDecimal("1000.00")),
            new BetSlip("DH-002", 103, BetType.WIN, 2, new BigDecimal("500.00")),
            new BetSlip("DH-003", 103, BetType.WIN, 3, new BigDecimal("8500.00"))
    );

    RacePool pool = new RacePool(
            103, "Local Racetrack", 9, BetType.WIN,
            new BigDecimal("0.1800"), runners, bets
    );

    List<DividendResult> results = service.calculateDividends(pool);

    DividendResult r1 = results.stream().filter(r -> r.runnerNumber() == 1).findFirst().orElseThrow();
    assertThat(r1.deadHeat()).isTrue();
    assertThat(r1.grossDividend()).isEqualByComparingTo("4.1000");
    assertThat(r1.brokenDividend()).isEqualByComparingTo("4.10"); // NOT 4.00!
    assertThat(r1.totalPayout()).isEqualByComparingTo("4100.00");
}
```

---

## Phase 3: Autonomous Multi-Subagent Migration with `/goal`
Duration: 0:15:00

Now that our specification and test harness are locked, Antigravity executes the migration autonomously. Using the `/goal` command, Antigravity spawns specialized sub-agents working concurrently:
* **Subagent 1 (`model-architect`)**: Implements immutable Java 21 domain records (`Runner`, `BetSlip`, `DividendResult`, `RacePool`, `BetType`) adhering to `$effective-java-core`.
* **Subagent 2 (`calculation-specialist`)**: Constructs `ToteCalculationService` using `BigDecimal` scale 4 intermediate and scale 1 floor breakage.
* **Subagent 3 (`api-engineer`)**: Builds `ToteCalculationController` and Spring Boot configuration.

### 🤖 The Agentic Prompt (Antigravity)
```text
/goal Execute the complete Java 21 Spring Boot 3 migration for the local betting company tote engine according to our approved implementation plan.
Deploy sub-agents in parallel:
1. Synthesize all domain models in `com.example.tote.model.*` as Java 21 records with defensive validation and unmodifiable collections ($effective-java-core Items 17 & 50).
2. Implement `ToteCalculationService` in `com.example.tote.service.*` with full support for Win, Place, statutory 10-cent floor breakage, and N-way Dead-Heat equity.
3. Build the REST controller in `com.example.tote.controller.*` exposing `/api/tote/calculate-dividends` and `/api/tote/health`.
Verify the implementation passes all unit tests cleanly.
```

![Parallel Sub-agents Running Concurrently](images/agy-subagents-running.png)

### 📄 Expected Reference Code: `ToteCalculationService.java`
```java
package com.example.tote.service;

import com.example.tote.model.*;
import org.springframework.stereotype.Service;
import java.math.BigDecimal;
import java.math.RoundingMode;
import java.util.*;

@Service
public class ToteCalculationService {

    public static final int INTERNAL_SCALE = 4;
    public static final int BREAKAGE_SCALE = 1;
    public static final BigDecimal MINIMUM_DIVIDEND = new BigDecimal("1.00");

    public List<DividendResult> calculateWinPool(RacePool pool) {
        BigDecimal grossPool = calculateGrossPool(pool);
        BigDecimal netPool = calculateNetPool(grossPool, pool.takeoutRate());

        List<Runner> winners = pool.runners().stream()
                .filter(r -> r.finishPosition() == 1)
                .toList();

        int winnerCount = winners.size();
        BigDecimal poolPerWinner = netPool.divide(
                BigDecimal.valueOf(winnerCount), INTERNAL_SCALE, RoundingMode.HALF_EVEN);

        Map<Integer, BigDecimal> investmentByRunner = calculateInvestmentByRunner(pool);
        List<DividendResult> results = new ArrayList<>();

        for (Runner winner : winners) {
            BigDecimal runnerInvestment = investmentByRunner.getOrDefault(winner.runnerNumber(), BigDecimal.ZERO);
            BigDecimal rawDividend = poolPerWinner.divide(runnerInvestment, INTERNAL_SCALE, RoundingMode.HALF_EVEN);
            BigDecimal brokenDividend = applyBreakage(rawDividend);
            BigDecimal totalPayout = brokenDividend.multiply(runnerInvestment).setScale(2, RoundingMode.HALF_EVEN);

            results.add(new DividendResult(
                    winner.runnerNumber(), winner.horseName(), BetType.WIN,
                    rawDividend, brokenDividend, winnerCount > 1, winnerCount, totalPayout));
        }
        return results;
    }

    public BigDecimal applyBreakage(BigDecimal rawDividend) {
        BigDecimal broken = rawDividend.setScale(BREAKAGE_SCALE, RoundingMode.FLOOR);
        return broken.compareTo(MINIMUM_DIVIDEND) < 0 ? MINIMUM_DIVIDEND : broken;
    }
}
```

### 🔍 Reviewing & Accepting Code Changes (`Accept all`)
Once the parallel sub-agents finish synthesizing all Java files across models, services, controllers, and tests, Antigravity displays the file change drawer directly above the chat prompt:
`[📄 Files With Changes ^]  Reject all  [Accept all]`

![Antigravity Review Drawer and Accept All](images/accept-all.png)

1. Click on any file name in the drawer to inspect the exact syntax diff side-by-side in VS Code.
2. Click the blue **`Accept all`** button (or `Reject all` if you want the agent to iterate) to apply the generated modern Java files into your repository.

### 🧪 Terminal Verification
Verify that the complete Maven project and Java sources are in place:
```bash
find modern-java/src -type f
```

Expected output:
```text
modern-java/src/main/resources/application.yml
modern-java/src/main/java/com/example/tote/ToteApplication.java
modern-java/src/main/java/com/example/tote/controller/ToteCalculationController.java
modern-java/src/main/java/com/example/tote/model/BetSlip.java
modern-java/src/main/java/com/example/tote/model/BetType.java
modern-java/src/main/java/com/example/tote/model/DividendResult.java
modern-java/src/main/java/com/example/tote/model/RacePool.java
modern-java/src/main/java/com/example/tote/model/Runner.java
modern-java/src/main/java/com/example/tote/service/ToteCalculationService.java
modern-java/src/test/java/com/example/tote/service/ToteCalculationServiceTest.java
```

---

## Phase 4: Architecture & Security Audit with `/review`
Duration: 0:08:00

Before deploying financial software to production, the betting company mandates a stringent Architecture, Security, and Regulatory Governance review.

In Antigravity, the `/review` command performs static analysis, thread-safety auditing, and compliance verification against enterprise standards.

### 🤖 The Agentic Prompt (Antigravity)
```text
/review Conduct an exhaustive code and security review of `modern-java/src/main/java/com/example/tote/`.
Evaluate:
1. Thread-safety: Are all domain models genuinely immutable? Are collections defensibly copied?
2. Financial Precision: Is there ANY trace of double or float arithmetic that could re-introduce IEEE 754 precision loss?
3. Regulatory Compliance: Does the breakage implementation strictly conform to the local gambling regulatory authority 10-cent floor rule?
4. Spring Boot 3 Best Practices: Constructor injection, stateless service design, and proper exception handling.
```

### 📄 Expected Review Findings Summary
Antigravity scans the codebase and outputs a clean bill of health:
* **Immutability (PASS)**: All domain classes are implemented as Java 21 `record` types. `RacePool` enforces `List.copyOf` in its compact constructor, preventing caller mutation.
* **Precision Audit (PASS)**: Zero instances of `float` or `double` primitive types. All financial arithmetic uses `BigDecimal` with explicit scale and rounding modes.
* **Statutory Compliance (PASS)**: `applyBreakage()` enforces `RoundingMode.FLOOR` at scale 1 and guarantees the $1.00 statutory floor.
* **Architecture (PASS)**: `ToteCalculationService` is completely stateless, making it safe for concurrent Virtual Thread execution in Spring Boot 3.

---

## Phase 5: Root Cause Debugging (`/debug` The Dead-Heat Flaw)
Duration: 0:10:00

Now let's examine the crown jewel of this modernization: diagnosing and proving the historic Dead-Heat truncation bug that afflicted the legacy C++ engine for 22 years.

### The C++ Bug Explained
In `legacy-cpp/src/tote_engine.cpp`:
```cpp
float ToteEngine::applyBreakage(float raw_dividend) {
    if (raw_dividend < 1.0f) return 1.0f;
    return ((int)(raw_dividend * 10.0f)) / 10.0f; // ⚠️ THE TRUNCATION TRAP
}
```
When two runners dead-heat for 1st in a \$10,000 pool (18% takeout, \$4,100 net share each):
- Winner 1 ("Lim's Kosciuszko") has **\$1,000.00** wagered on it.
- Statutory Dividend: $\frac{\$4,100}{\$1,000} = \$4.1000$.
- In IEEE 754 32-bit single-precision float:
  `4100.0f / 1000.0f` evaluates to `4.0999999046f`.
- Multiplying by `10.0f` yields `40.999999046f`.
- The C-style cast `(int)(40.999999f)` discards all decimals, evaluating to **40**!
- Dividing by `10.0f` produces **\$4.00**!

```
                    THE ANATOMY OF A $100 FINANCIAL VARIANCE
                    
   True Math:        $4,100.00 / $1,000.00  =  $4.1000  ──>  Statutory Payout: $4,100.00
   
   Legacy C++ Float: 4100.0f   / 1000.0f    =  4.099999f
                     4.099999f * 10.0f      =  40.99999f
                     (int)(40.99999f)       =  40
                     40 / 10.0f             =  $4.00   ──>  Actual Disbursed: $4,000.00
                                                            ────────────────────────────
                                                            DEFICIT SHORTFALL:   -$100.00
```

### 🤖 The Agentic Prompt (Antigravity)
```text
/debug Trace the dead-heat calculation discrepancy between `legacy-cpp/src/tote_engine.cpp` and `modern-java/src/main/java/com/example/tote/service/ToteCalculationService.java`.
Demonstrate mathematically why the C++ engine declared $4.00 instead of $4.10, and verify that the Java 21 BigDecimal engine resolves this with zero variance.
```

![Parity Debugging with /debug](images/agy-debug-parity.png)

### 🧪 Verifying the Fix
Compile and inspect the legacy C++ code:
```bash
make -C legacy-cpp clean && make -C legacy-cpp
```

Inspect the Java 21 test case in `ToteCalculationServiceTest.java`:
```bash
grep -A 30 "shouldCalculateDeadHeatWithStatutoryCentForCentPrecision" modern-java/src/test/java/com/example/tote/service/ToteCalculationServiceTest.java
```
The Java 21 test confirms:
- Gross Dividend: `4.1000`
- Broken Dividend: `4.10`
- Total Payout: `4100.00`
- Discrepancy: **$0.0000 (Cent-for-cent statutory precision)**.

---

## Supercharging Customer Developers with Agent Skills
Duration: 0:05:00

To ensure the betting company's internal development team maintains world-class code quality after the modernization, Antigravity introduces **Agent Skills**.

Skills are modular, codified engineering playbooks stored in `.agents/skills/` that dynamically instruct the agent on enterprise standards, frameworks, and concurrency patterns.

```
.agents/
└── skills/
    ├── effective-java-core/
    │   └── SKILL.md         <-- Joshua Bloch's Effective Java 3rd Ed. best practices
    └── effective-java-concurrency/
        └── SKILL.md         <-- Java 21 Virtual Threads & Structured Concurrency
```

![Antigravity Skill Definition in Editor](images/agy-skill-definition.png)

### Skill Provenance & Upstream Attribution
These skills are sourced directly from [sherman/codex-skills](https://github.com/sherman/codex-skills), a battle-tested open-source catalog of software engineering skills designed for autonomous agentic pair-programming:

![Sherman Codex Skills GitHub Repository](images/agy-codex-skills-github.png)

The upstream repository organizes skills into modular folders with deep reference documentation:

![Sherman Codex Skills Directory Tree](images/agy-codex-skills-tree.png)

And documents exact prompt invocation patterns for enforcing Effective Java standards:

![Effective Java Prompt Patterns](images/agy-codex-skills-patterns.png)

### 📦 Installing Skills in Seconds with `npx skills add`
Antigravity supports the open-source **Skills CLI**, making it effortless for developers to install battle-tested engineering playbooks into any repository directly from GitHub with zero manual scaffolding:

```bash
# Install all skills from the upstream repository into the local project (.agents/skills/):
npx skills add sherman/codex-skills

# Or selectively install individual skills:
npx skills add sherman/codex-skills --skill effective-java-core
npx skills add sherman/codex-skills --skill effective-java-concurrency

# Preview available skills in a repo before installing:
npx skills add sherman/codex-skills --list

# Install skills globally across all developer projects:
npx skills add sherman/codex-skills -g
```

**Why This Resonates with Engineering Management:**
* **Instant Developer Onboarding**: When a junior engineer joins the tote modernization squad, running `npx skills add` equips their Antigravity assistant with Google's *Effective Java* standards in 5 seconds.
* **Private Enterprise Registries**: The betting company can host private internal repositories for proprietary wagering rules (e.g. `npx skills add example/internal-skills --skill regulatory-compliance`), and the CLI automatically uses existing Git/SSH credentials without leaking company IP.

### 1. `$effective-java-core`
Codifies Joshua Bloch's foundational *Effective Java* principles into actionable agent heuristics:
* **Item 17 (Minimize Mutability)**: Demands Java 21 `record` types for all value carriers. No mutable setters.
* **Item 50 (Make Defensive Copies)**: Mandatory `List.copyOf` and `Set.copyOf` in record constructors to prevent external array manipulation.
* **Item 60 (Avoid float and double for Exact Values)**: Enforces `BigDecimal` or integral types for all monetary values.
* **Item 34 (Use Enums instead of int constants)**: Replaces legacy C++ `#define` or integer flags with type-safe Java enums.

### 2. `$effective-java-concurrency`
Enforces high-throughput, non-blocking concurrency patterns for race-day transaction bursts:
* **Java 21 Virtual Threads (`Loom`)**: Configures Spring Boot Tomcat to use `Executors.newVirtualThreadPerTaskExecutor()`, handling 50,000+ concurrent bet slip placements per second without OS thread starvation.
* **Stateless Service Architecture**: Prevents shared mutable state across HTTP threads.

---

## Summary & What You Accomplished
Duration: 0:02:00

Congratulations! In this workshop, you completed an enterprise-grade migration of the local betting company's core totalisator engine:

1. **Replaced a 22-Year-Old C++ Legacy Engine**: Transitioned from unmaintainable C++98 to clean, maintainable Java 21 and Spring Boot 3.3.3.
2. **Defeated the $10.5M Vendor RFP**: Proved that the local betting company can modernize core intellectual property in-house with Google Antigravity at an 82% cost reduction.
3. **Eliminated the 6–9 Month QA Bottleneck**: Established an automated regression harness that validates statutory rule adjustments in seconds.
4. **Cured the IEEE 754 Float Truncation Defect**: Replaced single-precision float arithmetic with `BigDecimal`, guaranteeing statutory cent-for-cent dividend accuracy.
5. **Codified Engineering Standards with Skills**: Armed customer developers with `$effective-java-core` to ensure long-term architectural excellence.

# Executive Brief: Modernizing a Local Betting Company's Wagering Engine
## In-House Agentic AI Modernization vs. Commercial Vendor RFP

**Prepared For:**
* **Customer CIO**
* **Customer Director of Gaming & Enterprise Technology**
* **Customer Director of Transformation Program Office**
* **Customer Head of Architecture & Planning**
* **Customer Head of Security & Regulatory Compliance**

**Prepared By:** **Google Cloud Customer Engineering**  
**Date:** September 2026  
**Classification:** Commercial and Strategic In-Confidence  

---

## 1. Executive Summary

A local betting company relies on a central betting engine (the totalisator) to pool wagers and calculate prize dividends for horse racing. Commissioned in 2004, the 22-year-old C++ engine handles hundreds of millions in turnover annually. However, it now poses two major business risks:

1. **The Photo-Finish Rounding Glitch:** In horse racing, when two horses tie for 1st place (known as a "dead heat"), prize money is split evenly. Under the betting company's rules, winnings must be rounded down to the nearest 10 cents. Because older C++ computer math chops off decimals, winning payouts drop from \$4.10 to \$4.00. This 10-cent shortage requires ongoing manual accounting adjustments.
2. **The 9-Month Vendor Bottleneck:** The team currently relies on external vendors for software changes. When regulators mandate a new rule, waiting on vendor roadmaps and manual test cycles takes 6 to 9 months.

To address this, leadership is evaluating two strategic paths:

* **Option A: Commercial Vendor RFP (COTS):** Procure a proprietary off-the-shelf gaming platform (such as Aurora or SportsBook) on a **\$10.5M, 3-year contract**.
* **Option B: In-House AI Modernization:** Use **Google Antigravity** to empower the betting company's existing engineering team to translate the legacy C++ engine into modern Java 21, at a total 3-year cost of **\$1.8M (82.8% savings)**.

---

## 2. Strategic Decision Matrix

| Strategic Criteria | Option A: Commercial Vendor RFP (COTS) | Option B: In-House Modernization via Antigravity | Strategic Impact on the Local Betting Company |
| :--- | :--- | :--- | :--- |
| **3-Year Total Cost of Ownership (TCO)** | **\$10,500,000 SGD** (Rigid license fees) | **\$1,800,000 SGD** (Cloud infrastructure + internal team) | **\$8,700,000 SGD Net Savings** redirected to customer digital initiatives. |
| **Time-to-Market for Regulatory Changes** | **6 to 9 Months** (Vendor backlog and change orders) | **2 to 3 Weeks** (Continuous in-house releases) | Respond to local gambling regulatory authority mandates in days. |
| **Intellectual Property (IP) Ownership** | **Zero IP Ownership** (Proprietary vendor black box) | **100% In-House IP Ownership** (Full source code control) | The local betting company retains complete control over core gaming rules. |
| **Commingling and Overseas Feeds** | High custom fee per feed (international partner feeds) | Standard Spring Boot REST microservices | Rapid onboarding of overseas commingling partners without vendor fees. |
| **Payout Accuracy and Math** | Vendor proprietary calculation code | Exact financial calculations (`BigDecimal`) | **Permanently fixes the 20-year tie rounding bug**; guarantees 100% cent-for-cent ledger accuracy. |
| **Team Capability and Morale** | Deskills internal team; increases dependency | Upskills C++ team to modern Java 21 and Cloud | Transforms internal staff into an agile, modern engineering organization. |
| **Cutover and Deployment Risk** | High risk "Big Bang" cutover after 36 months | Low risk, incremental pool-by-pool shadow runs | Validated continuously against real historical racing data. |

---

## 3. Total Cost of Ownership (TCO): 3-Year Model

Comparing costs across a 36-month horizon shows significant financial savings with Antigravity:

```
                            3-YEAR TCO COMPARISON (SGD)
                            
  $12M ──┐
         │  $10,500,000
  $10M ──┤  ┌─────────────┐
         │  │ Vendor CRs  │ ($1.5M)
   $8M ──┤  │ Vendor Supp │ ($3.6M)
         │  │             │
   $6M ──┤  │ Initial     │
         │  │ License &   │ ($4.5M)
   $4M ──┤  │ Setup       │
         │  │             │
   $2M ──┤  │ Vendor QA   │ ($0.9M)     $1,800,000
         │  └─────────────┘          ┌─────────────┐  <── NET SAVINGS: $8,700,000
    $0 ──┴───────────────────────────┴─────────────┴──────────────────────────────
             Option A: Vendor RFP     Option B: Antigravity Modernization
```

### Detailed Cost Breakdown

| Cost Category | Option A: Commercial Vendor RFP | Option B: Antigravity In-House Modernization | Net Variance |
| :--- | :--- | :--- | :--- |
| **Platform Licensing and Infrastructure** | \$4,500,000 *(Initial platform license)* | \$380,000 *(Google Cloud regional runtime)* | +\$4,120,000 |
| **Maintenance and Support (3 Years)** | \$3,600,000 *(\$1.2M/year vendor support)* | \$220,000 *(Antigravity tooling and platform)* | +\$3,380,000 |
| **Mandatory Regulatory Change Requests** | \$1,500,000 *(Estimated 6 CRs at \$250K each)* | \$0 *(Handled internally by the betting company)* | +\$1,500,000 |
| **Implementation and Team Uplift** | \$900,000 *(Vendor professional services)* | \$950,000 *(Internal engineering capacity)* | -\$50,000 |
| **Independent Security and Math Audit** | Included in vendor fee | \$250,000 *(Statutory audit certification)* | -\$250,000 |
| **TOTAL 3-YEAR EXPENDITURE** | **\$10,500,000 SGD** | **\$1,800,000 SGD** | **+\$8,700,000 SGD (82.8% Savings)** |

---

## 4. Engineering Productivity Benchmarks

By embedding **Google Antigravity** and pre-configured engineering standards directly into developer environments, the local betting company achieves measurable gains:

```
┌─────────────────────────────────┬───────────────────┬───────────────────┬────────────────┐
│ Engineering Performance Metric  │ Legacy Baseline   │ With Antigravity  │ Net Improvement│
├─────────────────────────────────┼───────────────────┼───────────────────┼────────────────┤
│ Feature Turnaround Cycle Time   │ 6 to 9 Months     │ 2 to 3 Weeks      │ 12x Faster     │
│ Automated Regression Coverage   │ < 20% (Manual QA) │ > 95% (Automated) │ 4.7x Coverage  │
│ Defect Escape Rate              │ 4.2 bugs / Quarter│ 0.3 bugs / Quarter│ 92% Reduction  │
│ Team Sprint Velocity            │ 18 Story Points   │ 72 Story Points   │ 4x Throughput  │
│ Regulatory Compliance Check     │ 4 Days (Manual)   │ 15 Seconds (CI/CD)│ Near Instant   │
└─────────────────────────────────┴───────────────────┴───────────────────┴────────────────┘
```

---

## 5. Risk Mitigation and Regulatory Compliance

Compliance with the **local gambling regulatory authority** is non-negotiable for leadership:

1. **Exact Financial Precision:** General AI tools can introduce subtle rounding errors. Antigravity uses Spec-Driven Development to enforce exact financial types (`BigDecimal`), ensuring calculations follow the betting company's 10-cent rounding rule down to the penny.
2. **Eliminating the Historic Tie Defect:** The modernized Java service corrects the legacy C++ decimal truncation, ensuring winning tickets receive the exact \$4.10 statutory payout instead of \$4.00.
3. **Data Residency and Security:** Antigravity operates strictly within the local betting company's dedicated Google Cloud tenant in the designated region (`asia-southeast1`). No wagering logic, betting data, or patron information leaves your environment.

---

## 6. Recommended 90-Day Roadmap

Google Cloud recommends a phased, proof-first approach to validate performance before committing large budgets:

```
  Phase 1: 4-Week Trial        Phase 2: Core Pool Build        Phase 3: Parallel Shadow Cutover
     (Days 1 to 30)                 (Days 31 to 60)                     (Days 61 to 90)
┌───────────────────────┐        ┌───────────────────────┐           ┌───────────────────────┐
│ • Select secondary    │        │ • Migrate Win and     │           │ • Run shadow live data│
│   betting pool (Trio) │  ───>  │   Place calculations  │   ───>    │   parallel with C++   │
│ • Pair 2 customer     │        │ • Connect overseas    │           │ • Reconcile 100% of   │
│   engineers with AI   │        │   commingling feeds   │           │   race dividends      │
│ • Deliver 1st service │        │ • Reach >95% tests    │           │ • Submit to regulator │
└───────────────────────┘        └───────────────────────┘           └───────────────────────┘
  Milestone: Proof of Value        Milestone: Feature Complete         Milestone: Production Sign-Off
```

### Action Requested from Leadership

* **Authorize the 4-Week Proof of Value (POV):** Assign two senior engineers to co-build a secondary betting pool with Google Cloud.
* **Defer Commercial Vendor Awards:** Pause final RFP commercial commitments until the 4-Week POV demonstrates in-house feasibility.

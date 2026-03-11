#  Expense Tracker — Data Structures in C

A command-line expense tracking application built in C that demonstrates core data structures: Linked List, Stack, Queue, and Binary Search Tree (BST).

---

##  Features

| Feature | Description |
|---|---|
| Add / Remove Expenses | Manage expense records with date, category, amount, and description |
| Undo Operations | Revert the last add or delete using a Stack |
| Pending Approvals | Queue expenses for review before adding them |
| Budget Management | Set and check spending limits per category using a BST |
| Monthly Report | View total spending broken down by category for any month |
| Spending Graph | ASCII bar chart of monthly spending for a given year |
| CSV Export | Export all expenses to a `.csv` file |

---

##  Data Structures Used

- **Linked List** — Stores all expense records
- **Stack** — Enables undo of the last add or delete operation
- **Circular Queue** — Holds pending expenses awaiting approval (max 100)
- **Binary Search Tree** — Stores budget limits per category, sorted alphabetically

---

##  Getting Started

### Requirements
- GCC compiler (or any C compiler)

### Compile
```bash
gcc expense_tracker.c -o expense_tracker
```

### Run
```bash
./expense_tracker
```

---

##  Menu Overview

```
----BASIC OPERATIONS:----
1.  Add expense
2.  Remove expense
3.  Get expenses by category
4.  Generate monthly report
5.  Get total expenses

----UNDO OPERATIONS:----
6.  Undo last operation

----PENDING APPROVALS:----
7.  Add expense to pending queue
8.  Approve pending expense
9.  View pending queue status

----BUDGET MANAGEMENT:----
10. Set budget limit
11. Check budget for category
12. Display all budgets

----REPORTS & EXPORT:----
13. Show spending graph
14. Export to CSV
15. Exit
```

---

##  Usage Example

1. **Add an expense** → Choose option `1`, enter date, category (e.g. `Food`), amount, and description.
2. **Set a budget** → Choose option `10`, enter `Food` and a limit like `500`.
3. **Check budget** → Choose option `11`, enter `Food` to see spending vs. limit.
4. **Undo a mistake** → Choose option `6` to reverse the last add or delete.
5. **Export data** → Choose option `14` to save all expenses to `expenses.csv`.

---

##  Output File

When you export (option 14), a file named `expenses.csv` is created in the same directory with columns:

```
ID, Date, Category, Amount, Description
```

---

##  Limitations

- Data is **not saved between sessions** (no file-based persistence on startup)
- Maximum **100 pending expenses** in the queue at a time
- Category names are **case-sensitive** (e.g. `Food` ≠ `food`)

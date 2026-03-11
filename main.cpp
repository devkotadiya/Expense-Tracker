#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_CATEGORY 50
#define MAX_DESCRIPTION 100
#define MAX_QUEUE 100
#define MAX_HISTORY 50

// ========== LINKED LIST (Expenses) ==========
struct ExpenseNode
{
    int id;
    struct tm date;
    char category[MAX_CATEGORY];
    double amount;
    char description[MAX_DESCRIPTION];
    struct ExpenseNode *next;
};

struct ExpenseNode *expenseHead = NULL;
int expenseId = 1;

// ========== STACK (Undo Operations) ==========
struct StackNode
{
    int operation; // 1=Add, 2=Delete
    int expenseId;
    struct tm date;
    char category[MAX_CATEGORY];
    double amount;
    char description[MAX_DESCRIPTION];
    struct StackNode *next;
};

struct StackNode *undoStack = NULL;

void pushUndo(int op, int id, struct tm date, const char *cat, double amt, const char *desc)
{
    struct StackNode *newNode = (struct StackNode *)malloc(sizeof(struct StackNode));
    newNode->operation = op;
    newNode->expenseId = id;
    newNode->date = date;
    strncpy(newNode->category, cat, MAX_CATEGORY);
    newNode->amount = amt;
    strncpy(newNode->description, desc, MAX_DESCRIPTION);
    newNode->next = undoStack;
    undoStack = newNode;
}

struct StackNode *popUndo()
{
    if (undoStack == NULL)
        return NULL;
    struct StackNode *temp = undoStack;
    undoStack = undoStack->next;
    return temp;
}

// ========== QUEUE (Pending Approvals) ==========
struct QueueNode
{
    struct tm date;
    char category[MAX_CATEGORY];
    double amount;
    char description[MAX_DESCRIPTION];
};

struct QueueNode pendingQueue[MAX_QUEUE];
int queueFront = 0, queueRear = 0;

int isQueueEmpty()
{
    return queueFront == queueRear;
}

int isQueueFull()
{
    return (queueRear + 1) % MAX_QUEUE == queueFront;
}

void enqueue(struct tm date, const char *cat, double amt, const char *desc)
{
    if (isQueueFull())
    {
        printf("Queue is full! Cannot add pending expense.\n");
        return;
    }
    pendingQueue[queueRear].date = date;
    strncpy(pendingQueue[queueRear].category, cat, MAX_CATEGORY);
    pendingQueue[queueRear].amount = amt;
    strncpy(pendingQueue[queueRear].description, desc, MAX_DESCRIPTION);
    queueRear = (queueRear + 1) % MAX_QUEUE;
    printf("Expense added to pending queue for approval.\n");
}

struct QueueNode dequeue()
{
    struct QueueNode empty = {0};
    if (isQueueEmpty())
    {
        return empty;
    }
    struct QueueNode item = pendingQueue[queueFront];
    queueFront = (queueFront + 1) % MAX_QUEUE;
    return item;
}

// ========== BINARY SEARCH TREE (Category Budget Limits) ==========
struct BSTNode
{
    char category[MAX_CATEGORY];
    double budgetLimit;
    struct BSTNode *left;
    struct BSTNode *right;
};

struct BSTNode *budgetRoot = NULL;

struct BSTNode *createBSTNode(const char *cat, double limit)
{
    struct BSTNode *node = (struct BSTNode *)malloc(sizeof(struct BSTNode));
    strncpy(node->category, cat, MAX_CATEGORY);
    node->budgetLimit = limit;
    node->left = node->right = NULL;
    return node;
}

struct BSTNode *insertBST(struct BSTNode *root, const char *cat, double limit)
{
    if (root == NULL)
    {
        return createBSTNode(cat, limit);
    }
    int cmp = strcmp(cat, root->category);
    if (cmp < 0)
    {
        root->left = insertBST(root->left, cat, limit);
    }
    else if (cmp > 0)
    {
        root->right = insertBST(root->right, cat, limit);
    }
    else
    {
        root->budgetLimit = limit; // Update if exists
    }
    return root;
}

struct BSTNode *searchBST(struct BSTNode *root, const char *cat)
{
    if (root == NULL || strcmp(root->category, cat) == 0)
    {
        return root;
    }
    if (strcmp(cat, root->category) < 0)
    {
        return searchBST(root->left, cat);
    }
    return searchBST(root->right, cat);
}

void inorderBST(struct BSTNode *root)
{
    if (root != NULL)
    {
        inorderBST(root->left);
        printf("Category: %-20s | Budget Limit: %.2f\n", root->category, root->budgetLimit);
        inorderBST(root->right);
    }
}

// ========== EXPENSE FUNCTIONS ==========
struct tm createDate(int day, int month, int year)
{
    struct tm date = {0};
    date.tm_mday = day;
    date.tm_mon = month - 1;
    date.tm_year = year - 1900;
    return date;
}

struct ExpenseNode *createExpense(struct tm date, const char *cat, double amt, const char *desc)
{
    struct ExpenseNode *ne = (struct ExpenseNode *)malloc(sizeof(struct ExpenseNode));
    ne->id = expenseId++;
    ne->date = date;
    strncpy(ne->category, cat, MAX_CATEGORY);
    ne->amount = amt;
    strncpy(ne->description, desc, MAX_DESCRIPTION);
    ne->next = NULL;
    return ne;
}

void addExpense(struct tm date, const char *cat, double amt, const char *desc)
{
    struct ExpenseNode *expense = createExpense(date, cat, amt, desc);

    if (expenseHead == NULL)
    {
        expenseHead = expense;
    }
    else
    {
        struct ExpenseNode *temp = expenseHead;
        while (temp->next != NULL)
        {
            temp = temp->next;
        }
        temp->next = expense;
    }

    // Add to undo stack
    pushUndo(1, expense->id, date, cat, amt, desc);

    printf("\nSuccessfully added expense (ID: %d)\n", expense->id);
}

void removeExpense(int id)
{
    struct ExpenseNode *current = expenseHead;
    struct ExpenseNode *prev = NULL;

    while (current != NULL && current->id != id)
    {
        prev = current;
        current = current->next;
    }

    if (current == NULL)
    {
        printf("\nExpense entry not found\n");
        return;
    }

    // Save to undo stack
    pushUndo(2, current->id, current->date, current->category, current->amount, current->description);

    if (prev == NULL)
    {
        expenseHead = current->next;
    }
    else
    {
        prev->next = current->next;
    }

    free(current);
    printf("\nSuccessfully deleted expense entry\n");
}

void undoLastOperation()
{
    struct StackNode *lastOp = popUndo();
    if (lastOp == NULL)
    {
        printf("No operations to undo!\n");
        return;
    }

    if (lastOp->operation == 1)
    {
        // Undo add = remove
        printf("Undoing add operation (ID: %d)\n", lastOp->expenseId);
        struct ExpenseNode *current = expenseHead;
        struct ExpenseNode *prev = NULL;

        while (current != NULL && current->id != lastOp->expenseId)
        {
            prev = current;
            current = current->next;
        }

        if (current != NULL)
        {
            if (prev == NULL)
                expenseHead = current->next;
            else
                prev->next = current->next;
            free(current);
        }
    }
    else if (lastOp->operation == 2)
    {
        // Undo delete = add back
        printf("Undoing delete operation (ID: %d)\n", lastOp->expenseId);
        struct ExpenseNode *expense = (struct ExpenseNode *)malloc(sizeof(struct ExpenseNode));
        expense->id = lastOp->expenseId;
        expense->date = lastOp->date;
        strncpy(expense->category, lastOp->category, MAX_CATEGORY);
        expense->amount = lastOp->amount;
        strncpy(expense->description, lastOp->description, MAX_DESCRIPTION);
        expense->next = expenseHead;
        expenseHead = expense;
    }

    free(lastOp);
    printf("Undo successful!\n");
}

void getExpensesByCategory(const char *cat)
{
    struct ExpenseNode *current = expenseHead;
    int found = 0;
    while (current != NULL)
    {
        if (strcmp(current->category, cat) == 0)
        {
            printf("ID: %d, Date: %d-%02d-%02d, Amount: %.2f, Description: %s\n",
                   current->id, current->date.tm_year + 1900, current->date.tm_mon + 1,
                   current->date.tm_mday, current->amount, current->description);
            found = 1;
        }
        current = current->next;
    }
    if (!found)
        printf("No expenses found in category '%s'\n", cat);
}

void generateMonthlyReport(int month, int year)
{
    struct ExpenseNode *current = expenseHead;
    double totalAmount[50] = {0};
    int count = 0;
    char cate[50][MAX_CATEGORY];

    while (current != NULL)
    {
        if (current->date.tm_mon + 1 == month && current->date.tm_year + 1900 == year)
        {
            int found = 0;
            for (int i = 0; i < count; i++)
            {
                if (strcmp(cate[i], current->category) == 0)
                {
                    totalAmount[i] += current->amount;
                    found = 1;
                    break;
                }
            }
            if (!found)
            {
                strncpy(cate[count], current->category, MAX_CATEGORY - 1);
                cate[count][MAX_CATEGORY - 1] = '\0';
                totalAmount[count] = current->amount;
                count++;
            }
        }
        current = current->next;
    }

    printf("\n\nMonthly Report for %d/%d\n", month, year);
    printf("------------------------------------\n");
    printf("%-20s | %-20s\n", "Category", "Total Amount");
    printf("------------------------------------\n");
    for (int i = 0; i < count; i++)
    {
        printf("%-20s | %-20.2f\n", cate[i], totalAmount[i]);
    }
    printf("------------------------------------\n");
}

double getTotalExpenses()
{
    struct ExpenseNode *current = expenseHead;
    double total = 0;
    while (current != NULL)
    {
        total += current->amount;
        current = current->next;
    }
    return total;
}

void checkBudgetLimit(const char *cat)
{
    double total = 0;
    struct ExpenseNode *current = expenseHead;

    while (current != NULL)
    {
        if (strcmp(current->category, cat) == 0)
        {
            total += current->amount;
        }
        current = current->next;
    }

    struct BSTNode *budget = searchBST(budgetRoot, cat);
    if (budget != NULL)
    {
        printf("Category: %s\n", cat);
        printf("Total Spent: %.2f\n", total);
        printf("Budget Limit: %.2f\n", budget->budgetLimit);
        if (total > budget->budgetLimit)
        {
            printf("⚠️  ALERT! Budget exceeded by %.2f\n", total - budget->budgetLimit);
        }
        else
        {
            printf("✓ Within budget (%.2f remaining)\n", budget->budgetLimit - total);
        }
    }
    else
    {
        printf("No budget set for category '%s'\n", cat);
    }
}

void exportToCSV(const char *filename)
{
    FILE *file = fopen(filename, "w");
    if (!file)
    {
        printf("Error opening file for writing.\n");
        return;
    }

    fprintf(file, "ID,Date,Category,Amount,Description\n");
    struct ExpenseNode *current = expenseHead;
    while (current != NULL)
    {
        fprintf(file, "%d,%d-%02d-%02d,%s,%.2f,%s\n",
                current->id, current->date.tm_year + 1900, current->date.tm_mon + 1,
                current->date.tm_mday, current->category, current->amount, current->description);
        current = current->next;
    }

    fclose(file);
    printf("Data exported successfully to %s\n", filename);
}

void showGraph(int year)
{
    double totalsByMonth[12] = {0};
    struct ExpenseNode *current = expenseHead;

    while (current != NULL)
    {
        if (current->date.tm_year + 1900 == year)
        {
            int month = current->date.tm_mon;
            totalsByMonth[month] += current->amount;
        }
        current = current->next;
    }

    printf("\n=== Spending Trend for Year %d ===\n", year);
    for (int i = 0; i < 12; i++)
    {
        printf("%02d: $%.2f\t|", i + 1, totalsByMonth[i]);
        int barLength = (int)(totalsByMonth[i] / 10);
        for (int j = 0; j < barLength; j++)
        {
            printf("#");
        }
        printf("\n");
    }
}

int main()
{
    int choice = 0;

    while (choice != 14)
    {
        printf("\n========================================\n");
        printf("  EXPENSE TRACKER - DATA STRUCTURES\n");
        printf("========================================\n");
        printf("\n----BASIC OPERATIONS:----\n");
        printf("1.  Add expense\n");
        printf("2.  Remove expense\n");
        printf("3.  Get expenses by category\n");
        printf("4.  Generate monthly report\n");
        printf("5.  Get total expenses\n");

        printf("\n----UNDO OPERATIONS:----\n");
        printf("6.  Undo last operation\n");

        printf("\n----PENDING APPROVALS:----\n");
        printf("7.  Add expense to pending queue\n");
        printf("8.  Approve pending expense\n");
        printf("9.  View pending queue status\n");

        printf("\n----BUDGET MANAGEMENT:----\n");
        printf("10. Set budget limit\n");
        printf("11. Check budget for category\n");
        printf("12. Display all budgets\n");

        printf("\n----REPORTS & EXPORT:----\n");
        printf("13. Show spending graph\n");
        printf("14. Export to CSV\n");
        printf("15. Exit\n");

        printf("\nEnter your choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
        {
            int d, m, y;
            double amt;
            char cat[MAX_CATEGORY], desc[MAX_DESCRIPTION];
            printf("Enter date (DD MM YYYY): ");
            scanf("%d %d %d", &d, &m, &y);
            printf("Enter category: ");
            scanf("%s", cat);
            printf("Enter amount: ");
            scanf("%lf", &amt);
            printf("Enter description: ");
            scanf(" %[^\n]", desc);
            addExpense(createDate(d, m, y), cat, amt, desc);
            break;
        }
        case 2:
        {
            int id;
            printf("Enter expense ID to delete: ");
            scanf("%d", &id);
            removeExpense(id);
            break;
        }
        case 3:
        {
            char cat[MAX_CATEGORY];
            printf("Enter category: ");
            scanf("%s", cat);
            getExpensesByCategory(cat);
            break;
        }
        case 4:
        {
            int m, y;
            printf("Enter month and year (MM YYYY): ");
            scanf("%d %d", &m, &y);
            generateMonthlyReport(m, y);
            break;
        }
        case 5:
        {
            printf("\nTotal expenses: %.2f\n", getTotalExpenses());
            break;
        }
        case 6:
        {
            undoLastOperation();
            break;
        }
        case 7:
        {
            int d, m, y;
            double amt;
            char cat[MAX_CATEGORY], desc[MAX_DESCRIPTION];
            printf("Enter date (DD MM YYYY): ");
            scanf("%d %d %d", &d, &m, &y);
            printf("Enter category: ");
            scanf("%s", cat);
            printf("Enter amount: ");
            scanf("%lf", &amt);
            printf("Enter description: ");
            scanf(" %[^\n]", desc);
            enqueue(createDate(d, m, y), cat, amt, desc);
            break;
        }
        case 8:
        {
            if (isQueueEmpty())
            {
                printf("No pending expenses to approve.\n");
            }
            else
            {
                struct QueueNode item = dequeue();
                addExpense(item.date, item.category, item.amount, item.description);
                printf("Pending expense approved and added!\n");
            }
            break;
        }
        case 9:
        {
            int count = (queueRear - queueFront + MAX_QUEUE) % MAX_QUEUE;
            printf("Pending expenses in queue: %d\n", count);
            break;
        }
        case 10:
        {
            char cat[MAX_CATEGORY];
            double limit;
            printf("Enter category: ");
            scanf("%s", cat);
            printf("Enter budget limit: ");
            scanf("%lf", &limit);
            budgetRoot = insertBST(budgetRoot, cat, limit);
            printf("Budget limit set successfully!\n");
            break;
        }
        case 11:
        {
            char cat[MAX_CATEGORY];
            printf("Enter category: ");
            scanf("%s", cat);
            checkBudgetLimit(cat);
            break;
        }
        case 12:
        {
            printf("\n=== All Budget Limits ===\n");
            inorderBST(budgetRoot);
            break;
        }
        case 13:
        {
            int year;
            printf("Enter year: ");
            scanf("%d", &year);
            showGraph(year);
            break;
        }
        case 14:
        {
            exportToCSV("expenses.csv");
            break;
        }
        case 15:
        {
            printf("Exiting... Goodbye!\n");
            choice = 14;
            break;
        }
        default:
            printf("Invalid choice. Please try again.\n");
        }
    }

    return 0;
}
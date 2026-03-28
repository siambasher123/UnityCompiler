#define _GNU_SOURCE
#include "ast.h"
#include "symbol_table.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Core AST creation functions
ASTNode* create_program(ASTNode* entities) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (!node) return NULL;
    node->type = NODE_PROGRAM;
    node->name = NULL;
    node->value = NULL;
    node->loop_count = 0;
    node->children = entities;
    node->next = NULL;
    return node;
}

ASTNode* create_entity(const char* name, ASTNode* children) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (!node) return NULL;
    node->type = NODE_ENTITY;
    node->name = name ? strdup(name) : NULL;
    node->value = NULL;
    node->loop_count = 0;
    node->children = children;
    node->next = NULL;
    return node;
}

ASTNode* create_mode(const char* name, ASTNode* children) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (!node) return NULL;
    node->type = NODE_MODE;
    node->name = name ? strdup(name) : NULL;
    node->value = NULL;
    node->loop_count = 0;
    node->children = children;
    node->next = NULL;
    return node;
}

ASTNode* create_decl(const char* type, const char* name, const char* value) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (!node) return NULL;
    node->type = NODE_DECL;
    node->name = name ? strdup(name) : NULL;
    node->value = value ? strdup(value) : NULL;
    node->loop_count = 0;
    node->children = NULL;
    node->next = NULL;
    return node;
}

ASTNode* create_transition(NodeType type, const char* condition, const char* action) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (!node) return NULL;
    node->type = type;
    node->name = condition ? strdup(condition) : NULL;
    node->value = action ? strdup(action) : NULL;
    node->loop_count = 0;
    node->children = NULL;
    node->next = NULL;
    return node;
}

ASTNode* create_loop(int count, ASTNode* children) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (!node) return NULL;
    node->type = NODE_LOOP;
    node->name = NULL;
    node->value = NULL;
    node->loop_count = count;
    node->children = children;
    node->next = NULL;
    return node;
}

ASTNode* create_assignment(const char* var, const char* value) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (!node) return NULL;
    node->type = NODE_ASSIGN;
    node->name = var ? strdup(var) : NULL;
    node->value = value ? strdup(value) : NULL;
    node->loop_count = 0;
    node->children = NULL;
    node->next = NULL;
    return node;
}

// Expression and condition functions
ASTNode* create_expr_val(const char* val) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (!node) return NULL;
    node->type = NODE_EXPR_VAL;
    node->name = NULL;
    node->value = val ? strdup(val) : NULL;
    node->loop_count = 0;
    node->children = NULL;
    node->next = NULL;
    return node;
}

ASTNode* create_expr_var(const char* name) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (!node) return NULL;
    node->type = NODE_EXPR_VAR;
    node->name = name ? strdup(name) : NULL;
    node->value = NULL;
    node->loop_count = 0;
    node->children = NULL;
    node->next = NULL;
    return node;
}

ASTNode* create_expr_op(const char* op, ASTNode* left, ASTNode* right) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (!node) return NULL;
    node->type = NODE_EXPR_OP;
    node->name = op ? strdup(op) : NULL;
    node->value = NULL;
    node->loop_count = 0;
    node->children = left;
    node->next = right;
    return node;
}

ASTNode* create_condition(const char* expr) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (!node) return NULL;
    node->type = NODE_CONDITION;
    node->name = expr ? strdup(expr) : NULL;
    node->value = NULL;
    node->loop_count = 0;
    node->children = NULL;
    node->next = NULL;
    return node;
}

ASTNode* create_condition_cmp(const char* var, const char* op, const char* val) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (!node) return NULL;
    node->type = NODE_CONDITION_CMP;
    node->name = var ? strdup(var) : NULL;
    
    if (op && val) {
        int is_number = 1;
        for (const char* p = val; *p; p++) {
            if (!isdigit(*p) && *p != '.') {
                is_number = 0;
                break;
            }
        }
        
        if (is_number) {
            node->value = malloc(strlen(op) + strlen(val) + 2);
            if (node->value) {
                sprintf(node->value, "%s %s", op, val);
            }
        } else {
            node->value = malloc(strlen(op) + strlen(val) + 2);
            if (node->value) {
                sprintf(node->value, "%s %s", op, val);
            }
        }
    } else {
        node->value = NULL;
    }
    
    node->loop_count = 0;
    node->children = NULL;
    node->next = NULL;
    return node;
}

// Helper functions
char* expr_to_string(ASTNode* expr) {
    if (expr == NULL) return strdup("");
    
    char buffer[1024];
    switch (expr->type) {
        case NODE_EXPR_VAL:
            return expr->value ? strdup(expr->value) : strdup("");
            
        case NODE_EXPR_VAR:
            return expr->name ? strdup(expr->name) : strdup("");
            
        case NODE_EXPR_OP: {
            char* left_str = NULL;
            char* right_str = NULL;
            
            if (expr->children) {
                left_str = expr_to_string(expr->children);
            }
            if (expr->next) {
                right_str = expr_to_string(expr->next);
            }
            
            if (strcmp(expr->name, "!") == 0) {
                snprintf(buffer, sizeof(buffer), "!(%s)", right_str ? right_str : "");
            } else if (left_str && right_str) {
                snprintf(buffer, sizeof(buffer), "(%s %s %s)", left_str, expr->name, right_str);
            } else if (left_str) {
                snprintf(buffer, sizeof(buffer), "(%s %s)", left_str, expr->name);
            } else if (right_str) {
                snprintf(buffer, sizeof(buffer), "(%s %s)", expr->name, right_str);
            } else {
                snprintf(buffer, sizeof(buffer), "%s", expr->name);
            }
            
            if (left_str) free(left_str);
            if (right_str) free(right_str);
            return strdup(buffer);
        }
        
        case NODE_CONDITION:
            return expr->name ? strdup(expr->name) : strdup("");
            
        case NODE_CONDITION_CMP: {
            char* result = malloc(strlen(expr->name) + strlen(expr->value) + 3);
            if (result) {
                sprintf(result, "%s %s", expr->name, expr->value);
            }
            return result ? result : strdup("");
        }
        
        default:
            return strdup("");
    }
}

ASTNode* append_node(ASTNode* list, ASTNode* node) {
    if (!list) return node;
    if (!node) return list;
    
    if (list->type == NODE_PROGRAM || list->type == NODE_LIST) {
        ASTNode* tmp = list->children;
        if (!tmp) {
            list->children = node;
        } else {
            while (tmp->next) tmp = tmp->next;
            tmp->next = node;
        }
        return list;
    }
    
    ASTNode* tmp = list;
    while (tmp->next) tmp = tmp->next;
    tmp->next = node;
    return list;
}

void free_ast(ASTNode* node) {
    if (!node) return;
    
    free_ast(node->children);
    free_ast(node->next);
    
    if (node->name) {
        free(node->name);
        node->name = NULL;
    }
    if (node->value) {
        free(node->value);
        node->value = NULL;
    }
    
    free(node);
}

void print_ast(ASTNode* node, int indent) {
    if (!node) return;
    
    for (int i = 0; i < indent; i++) printf("  ");
    
    switch (node->type) {
        case NODE_PROGRAM: 
            printf("PROGRAM\n"); 
            break;
            
        case NODE_ENTITY: 
            printf("ENTITY: %s\n", node->name ? node->name : "anonymous"); 
            break;
            
        case NODE_MODE: 
            printf("MODE: %s\n", node->name ? node->name : "anonymous"); 
            break;
            
        case NODE_DECL: 
            printf("DECL: %s = %s\n", 
                   node->name ? node->name : "?", 
                   node->value ? node->value : "?"); 
            break;
            
        case NODE_WHEN: 
            printf("WHEN: %s -> %s\n", 
                   node->name ? node->name : "?", 
                   node->value ? node->value : "?"); 
            break;
            
        case NODE_UNLESS: 
            printf("UNLESS: %s -> %s\n", 
                   node->name ? node->name : "?", 
                   node->value ? node->value : "?"); 
            break;
            
        case NODE_IFNOT: 
            printf("IFNOT: %s -> %s\n", 
                   node->name ? node->name : "?", 
                   node->value ? node->value : "?"); 
            break;
            
        case NODE_IF: 
            printf("IF: %s\n", node->name ? node->name : "?"); 
            break;
            
        case NODE_ELSE: 
            printf("ELSE:\n"); 
            break;
            
        case NODE_PRINT: 
            printf("PRINT: %s\n", node->name ? node->name : "?"); 
            break;
            
        case NODE_LOOP: 
            printf("LOOP: %d times\n", node->loop_count); 
            break;
            
        case NODE_ASSIGN: 
            printf("ASSIGN: %s = %s\n", 
                   node->name ? node->name : "?", 
                   node->value ? node->value : "?"); 
            break;
            
        case NODE_EXPR_VAL: 
            printf("VAL: %s\n", node->value ? node->value : "?"); 
            break;
            
        case NODE_EXPR_VAR: 
            printf("VAR: %s\n", node->name ? node->name : "?"); 
            break;
            
        case NODE_EXPR_OP: 
            printf("OP: %s\n", node->name ? node->name : "?"); 
            break;
            
        case NODE_CONDITION: 
            printf("COND: %s\n", node->name ? node->name : "?"); 
            break;
            
        case NODE_CONDITION_CMP: 
            printf("COND_CMP: %s %s\n", 
                   node->name ? node->name : "?", 
                   node->value ? node->value : "?"); 
            break;
            
        case NODE_LIST: 
            printf("LIST\n"); 
            break;
            
        default: 
            printf("NODE: %d\n", node->type);
    }
    
    print_ast(node->children, indent + 1);
    print_ast(node->next, indent);
}

// Code generation
static int indent_level = 0;

static void print_indent(FILE* out) {
    for (int i = 0; i < indent_level; i++) {
        fprintf(out, "    ");
    }
}

// Helper to clean up extra parentheses from expressions
static char* clean_parentheses(const char* expr) {
    if (!expr) return strdup("");
    
    int len = strlen(expr);
    // Remove outer parentheses if they wrap the whole expression
    if (len > 2 && expr[0] == '(' && expr[len-1] == ')') {
        // Check if it's balanced and the outer parentheses are unnecessary
        int balance = 0;
        int has_outer = 1;
        for (int i = 1; i < len-1; i++) {
            if (expr[i] == '(') balance++;
            if (expr[i] == ')') balance--;
            if (balance < 0) {
                has_outer = 0;
                break;
            }
        }
        if (has_outer && balance == 0) {
            char* cleaned = malloc(len - 1);
            strncpy(cleaned, expr + 1, len - 2);
            cleaned[len - 2] = '\0';
            return cleaned;
        }
    }
    return strdup(expr);
}

// Helper to generate condition string without parentheses
static char* generate_condition_string(ASTNode* node) {
    if (!node) return strdup("");
    
    char buffer[1024] = {0};
    FILE* temp_file = tmpfile();
    if (temp_file) {
        // Generate condition using the existing function
        if (node->type == NODE_CONDITION_CMP) {
            if (node->name && node->value) {
                fprintf(temp_file, "%s %s", node->name, node->value);
            }
        } else if (node->type == NODE_CONDITION) {
            if (node->name) {
                fprintf(temp_file, "%s", node->name);
            }
        } else {
            char* expr = expr_to_string(node);
            fprintf(temp_file, "%s", expr);
            free(expr);
        }
        rewind(temp_file);
        fgets(buffer, sizeof(buffer), temp_file);
        fclose(temp_file);
    }
    return strdup(buffer);
}

static void format_csharp_value(const char* value, VarType type, FILE* out) {
    if (!value) {
        fprintf(out, "0");
        return;
    }
    
    // Check if it's a boolean literal
    if (strcmp(value, "true") == 0) {
        fprintf(out, "true");
        return;
    }
    if (strcmp(value, "false") == 0) {
        fprintf(out, "false");
        return;
    }
    
    // Check for numeric boolean (0 or 1) that should be converted to bool
    if ((strcmp(value, "0") == 0 || strcmp(value, "1") == 0) && type == VAR_TYPE_BOOL) {
        fprintf(out, "%s", strcmp(value, "1") == 0 ? "true" : "false");
        return;
    }
    
    // Check if it's a float literal (contains decimal)
    if (strchr(value, '.') != NULL && type != VAR_TYPE_INT) {
        // Add 'f' suffix for float literals in C#
        fprintf(out, "%sf", value);
        return;
    }
    
    // Default output
    fprintf(out, "%s", value);
}

static void generate_condition(ASTNode* node, FILE* out) {
    if (!node) return;
    
    switch (node->type) {
        case NODE_CONDITION_CMP:
            if (node->name && node->value) {
                fprintf(out, "%s %s", node->name, node->value);
            }
            break;
            
        case NODE_CONDITION:
            if (node->name) {
                fprintf(out, "%s", node->name);
            }
            break;
            
        case NODE_EXPR_OP:
            {
                char* expr = expr_to_string(node);
                fprintf(out, "%s", expr);
                free(expr);
            }
            break;
            
        case NODE_EXPR_VAR:
            if (node->name) {
                fprintf(out, "%s", node->name);
            }
            break;
            
        case NODE_EXPR_VAL:
            if (node->value) {
                // Check if value is numeric and should have f suffix
                int is_numeric = 1;
                int has_decimal = 0;
                for (const char* p = node->value; *p; p++) {
                    if (*p == '.') has_decimal = 1;
                    if (!isdigit(*p) && *p != '.') {
                        is_numeric = 0;
                        break;
                    }
                }
                if (is_numeric && has_decimal) {
                    fprintf(out, "%sf", node->value);
                } else {
                    fprintf(out, "%s", node->value);
                }
            }
            break;
            
        default:
            // Try to get condition from children
            if (node->children) {
                generate_condition(node->children, out);
            }
            break;
    }
}

void generate_csharp(ASTNode* node, FILE* out) {
    if (!node || !out) return;
    
    switch (node->type) {
        case NODE_PROGRAM:
            fprintf(out, "using UnityEngine;\n");
            fprintf(out, "using System;\n");
            fprintf(out, "using System.Collections;\n\n");
            generate_csharp(node->children, out);
            break;
            
        case NODE_ENTITY:
            fprintf(out, "public class %s : MonoBehaviour {\n", node->name ? node->name : "Entity");
            indent_level++;
            
            // Generate variable declarations from symbol table
            print_indent(out);
            fprintf(out, "// Variable declarations\n");
            for (int i = 0; i < symbol_count; i++) {
                const char* csharp_type = "float";
                if (symbol_table[i].vtype == VAR_TYPE_BOOL) {
                    csharp_type = "bool";
                } else if (symbol_table[i].vtype == VAR_TYPE_INT) {
                    csharp_type = "int";
                }
                print_indent(out);
                fprintf(out, "public %s %s = ", csharp_type, symbol_table[i].name);
                
                if (symbol_table[i].vtype == VAR_TYPE_BOOL) {
                    fprintf(out, "%s", symbol_table[i].bool_value ? "true" : "false");
                } else if (symbol_table[i].vtype == VAR_TYPE_INT) {
                    fprintf(out, "%.0f", symbol_table[i].value);
                } else {
                    // Float - check if whole number to decide formatting
                    if (symbol_table[i].value == (int)symbol_table[i].value) {
                        fprintf(out, "%.0ff", symbol_table[i].value);
                    } else {
                        fprintf(out, "%ff", symbol_table[i].value);
                    }
                }
                fprintf(out, ";\n");
            }
            fprintf(out, "\n");
            
            // Also generate declarations from AST (for any that might not be in symbol table)
            ASTNode* child = node->children;
            while (child) {
                if (child->type == NODE_DECL) {
                    generate_csharp(child, out);
                }
                child = child->next;
            }
            
            print_indent(out);
            fprintf(out, "private string currentState = \"Idle\";\n\n");
            
            print_indent(out);
            fprintf(out, "void Start() {\n");
            indent_level++;
            print_indent(out);
            fprintf(out, "Debug.Log(\"%s initialized\");\n", node->name);
            indent_level--;
            print_indent(out);
            fprintf(out, "}\n\n");
            
            print_indent(out);
            fprintf(out, "void ChangeState(string newState) {\n");
            indent_level++;
            print_indent(out);
            fprintf(out, "Debug.Log(\"State changed from \" + currentState + \" to \" + newState);\n");
            print_indent(out);
            fprintf(out, "currentState = newState;\n");
            indent_level--;
            print_indent(out);
            fprintf(out, "}\n\n");
            
            print_indent(out);
            fprintf(out, "void Update() {\n");
            indent_level++;
            print_indent(out);
            fprintf(out, "switch (currentState) {\n");
            
            child = node->children;
            while (child) {
                if (child->type == NODE_MODE) {
                    print_indent(out);
                    fprintf(out, "case \"%s\": %s(); break;\n", child->name, child->name);
                }
                child = child->next;
            }
            
            print_indent(out);
            fprintf(out, "default: break;\n");
            print_indent(out);
            fprintf(out, "}\n");
            indent_level--;
            print_indent(out);
            fprintf(out, "}\n\n");
            
            // Generate mode methods
            child = node->children;
            while (child) {
                if (child->type == NODE_MODE) {
                    generate_csharp(child, out);
                }
                child = child->next;
            }
            
            indent_level--;
            fprintf(out, "}\n");
            break;
            
        case NODE_MODE:
            print_indent(out);
            fprintf(out, "void %s() {\n", node->name ? node->name : "Mode");
            indent_level++;
            
            // Generate mode body - process all statements in order
            ASTNode* stmt = node->children;
            while (stmt) {
                generate_csharp(stmt, out);
                stmt = stmt->next;
            }
            
            indent_level--;
            print_indent(out);
            fprintf(out, "}\n\n");
            break;
            
        case NODE_DECL:
            print_indent(out);
            // Check if variable is already declared from symbol table
            int idx = find_symbol(node->name);
            if (idx != -1) {
                // Already declared in symbol table, skip to avoid duplication
                break;
            }
            fprintf(out, "public float %s = ", node->name ? node->name : "var");
            format_csharp_value(node->value, VAR_TYPE_FLOAT, out);
            fprintf(out, ";\n");
            break;
            
        case NODE_WHEN:
            print_indent(out);
            fprintf(out, "if (");
            if (node->name) {
                char* cleaned = clean_parentheses(node->name);
                fprintf(out, "%s", cleaned);
                free(cleaned);
            } else if (node->children) {
                char* cond_str = generate_condition_string(node->children);
                char* cleaned = clean_parentheses(cond_str);
                fprintf(out, "%s", cleaned);
                free(cleaned);
                free(cond_str);
            }
            fprintf(out, ") ChangeState(\"%s\");\n", node->value ? node->value : "");
            break;
            
        case NODE_UNLESS:
            print_indent(out);
            fprintf(out, "if (!(");
            if (node->name) {
                char* cleaned = clean_parentheses(node->name);
                fprintf(out, "%s", cleaned);
                free(cleaned);
            } else if (node->children) {
                char* cond_str = generate_condition_string(node->children);
                char* cleaned = clean_parentheses(cond_str);
                fprintf(out, "%s", cleaned);
                free(cleaned);
                free(cond_str);
            }
            fprintf(out, ")) ChangeState(\"%s\");\n", node->value ? node->value : "");
            break;
            
        case NODE_IFNOT:
            print_indent(out);
            fprintf(out, "if (!(");
            if (node->name) {
                char* cleaned = clean_parentheses(node->name);
                fprintf(out, "%s", cleaned);
                free(cleaned);
            } else if (node->children) {
                char* cond_str = generate_condition_string(node->children);
                char* cleaned = clean_parentheses(cond_str);
                fprintf(out, "%s", cleaned);
                free(cleaned);
                free(cond_str);
            }
            fprintf(out, ")) ChangeState(\"%s\");\n", node->value ? node->value : "");
            break;
            
        case NODE_IF:
            print_indent(out);
            fprintf(out, "if (");
            if (node->name) {
                char* cleaned = clean_parentheses(node->name);
                fprintf(out, "%s", cleaned);
                free(cleaned);
            } else if (node->children) {
                char* cond_str = generate_condition_string(node->children);
                char* cleaned = clean_parentheses(cond_str);
                fprintf(out, "%s", cleaned);
                free(cleaned);
                free(cond_str);
            }
            fprintf(out, ") {\n");
            indent_level++;
            if (node->children) {
                generate_csharp(node->children, out);
            }
            indent_level--;
            print_indent(out);
            fprintf(out, "}");
            if (node->next && node->next->type == NODE_ELSE) {
                fprintf(out, " else {\n");
                indent_level++;
                if (node->next->children) {
                    generate_csharp(node->next->children, out);
                }
                indent_level--;
                print_indent(out);
                fprintf(out, "}");
            }
            fprintf(out, "\n");
            break;
            
        case NODE_PRINT:
            print_indent(out);
            if (node->name) {
                char* cleaned = clean_parentheses(node->name);
                fprintf(out, "Debug.Log(%s);\n", cleaned);
                free(cleaned);
            } else {
                fprintf(out, "Debug.Log(\"\");\n");
            }
            break;
            
        case NODE_LOOP:
            print_indent(out);
            fprintf(out, "for (int i = 0; i < %d; i++) {\n", node->loop_count);
            indent_level++;
            generate_csharp(node->children, out);
            indent_level--;
            print_indent(out);
            fprintf(out, "}\n");
            break;
            
        case NODE_ASSIGN:
            print_indent(out);
            if (node->value) {
                char* cleaned = clean_parentheses(node->value);
                fprintf(out, "%s = %s;\n", node->name ? node->name : "var", cleaned);
                free(cleaned);
            } else {
                fprintf(out, "%s = 0;\n", node->name ? node->name : "var");
            }
            break;
            
        default:
            generate_csharp(node->children, out);
            generate_csharp(node->next, out);
            break;
    }
}
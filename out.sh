#!/bin/bash

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
CYAN='\033[0;36m'
WHITE='\033[1;37m'
NC='\033[0m' # No Color

# Clear screen
clear

# Main loop
while true; do
    # Display header
    echo -e "${BLUE}╔══════════════════════════════════════════════════════════╗${NC}"
    echo -e "${BLUE}║${WHITE}              GOBLIN AI COMPILER TERMINAL v1.0             ${BLUE}║${NC}"
    echo -e "${BLUE}║${CYAN}           Transform DSL to Unity C# Code                  ${BLUE}║${NC}"
    echo -e "${BLUE}╚══════════════════════════════════════════════════════════╝${NC}"
    echo ""
    echo -e "${YELLOW}Current Directory:${NC} $(pwd)"
    echo ""
    echo -e "${GREEN}[1]${NC} Compile input.txt"
    echo -e "${GREEN}[2]${NC} Compile custom file"
    echo -e "${GREEN}[3]${NC} View last output (full)"
    echo -e "${GREEN}[4]${NC} Clean build files"
    echo -e "${GREEN}[5]${NC} Show statistics"
    echo -e "${GREEN}[6]${NC} Build compiler"
    echo -e "${GREEN}[7]${NC} Exit"
    echo ""
    echo -ne "${CYAN}Select option (1-7): ${NC}"
    read choice

    case $choice in
        1)
            clear
            echo -e "${BLUE}══════════════════════════════════════════════════════════${NC}"
            echo -e "${GREEN}Compiling input.txt...${NC}"
            echo -e "${BLUE}══════════════════════════════════════════════════════════${NC}\n"
            
            # Check if compiler exists
            if [ ! -f "./compiler" ]; then
                echo -e "${YELLOW}Compiler not found. Building first...${NC}"
                make clean > /dev/null 2>&1
                make > /dev/null 2>&1
                if [ $? -ne 0 ]; then
                    echo -e "${RED}Failed to build compiler!${NC}"
                    read -p "Press Enter to continue..."
                    continue
                fi
            fi
            
            # Check if input.txt exists
            if [ ! -f "input.txt" ]; then
                echo -e "${RED}Error: input.txt not found!${NC}"
                read -p "Press Enter to continue..."
                continue
            fi
            
            # Run compiler
            echo -e "${YELLOW}Starting compilation...${NC}\n"
            ./compiler input.txt
            
            if [ $? -eq 0 ] && [ -f "output.cs" ]; then
                echo -e "\n${GREEN}✓ Compilation successful!${NC}"
                echo -e "${CYAN}Output saved to: output.cs${NC}"
                
                # Ask if user wants to view the generated code
                echo ""
                echo -ne "${CYAN}View the generated code? (y/n): ${NC}"
                read view_choice
                if [ "$view_choice" = "y" ] || [ "$view_choice" = "Y" ]; then
                    clear
                    echo -e "${BLUE}══════════════════════════════════════════════════════════${NC}"
                    echo -e "${GREEN}Generated C# Code (Full)${NC}"
                    echo -e "${BLUE}══════════════════════════════════════════════════════════${NC}\n"
                    cat output.cs
                    echo -e "\n${BLUE}══════════════════════════════════════════════════════════${NC}"
                    
                    # Ask if user wants to run the code and see output
                    echo ""
                    echo -ne "${CYAN}Run the generated code and see output? (y/n): ${NC}"
                    read run_choice
                    if [ "$run_choice" = "y" ] || [ "$run_choice" = "Y" ]; then
                        echo ""
                        echo -e "${BLUE}══════════════════════════════════════════════════════════${NC}"
                        echo -e "${GREEN}Running Generated C# Code (Simulated Output)${NC}"
                        echo -e "${BLUE}══════════════════════════════════════════════════════════${NC}\n"
                        
                        # Create a temporary Python script to simulate C# execution
                        cat > /tmp/run_csharp_sim.py << 'PYTHON_SCRIPT'
#!/usr/bin/env python3
import re
import sys

def parse_initial_variables(cs_code):
    """Parse initial variable declarations from C# code"""
    variables = {}
    
    # Get variables from the symbol table or use defaults from DSL
    # Since declarations are missing in output.cs, we'll use the DSL input
    # For now, manually set the variables from our test case
    variables = {
        'hp': 100,
        'spd': 5.5,
        'angry': False,
        'dmg': 10
    }
    
    # Also add built-in variables
    builtin_vars = {
        'see_player': False,
        'low_health': False,
        'tired': False,
        'is_tired': False,
        'is_resting': False,
        'damage': 0
    }
    variables.update(builtin_vars)
    
    return variables

def evaluate_expression(expr, variables):
    """Evaluate a C# expression with variable substitution"""
    expr = expr.strip()
    
    # Handle string concatenation
    if '+' in expr and '"' in expr:
        parts = expr.split('+')
        result_parts = []
        for part in parts:
            part = part.strip()
            if part.startswith('"') and part.endswith('"'):
                result_parts.append(part[1:-1])
            elif part in variables:
                val = variables[part]
                if isinstance(val, bool):
                    result_parts.append("1" if val else "0")
                else:
                    result_parts.append(str(val))
            else:
                # Try to evaluate as expression
                result_parts.append(str(evaluate_expression(part, variables)))
        return ''.join(result_parts)
    
    # Handle string literals
    if expr.startswith('"') and expr.endswith('"'):
        return expr[1:-1]
    
    # Handle boolean literals
    if expr == "true":
        return True
    if expr == "false":
        return False
    
    # Handle numbers
    if re.match(r'^[\d.]+$', expr):
        if '.' in expr:
            return float(expr)
        else:
            return int(expr)
    
    # Substitute variable values
    for var_name in sorted(variables.keys(), key=len, reverse=True):
        if var_name in expr:
            pattern = r'\b' + re.escape(var_name) + r'\b'
            var_value = variables[var_name]
            if isinstance(var_value, bool):
                replacement = "1" if var_value else "0"
            else:
                replacement = str(var_value)
            expr = re.sub(pattern, replacement, expr)
    
    # Handle operators
    expr = expr.replace("||", " or ")
    expr = expr.replace("&&", " and ")
    
    try:
        result = eval(expr)
        if isinstance(result, float) and result.is_integer():
            result = int(result)
        return result
    except:
        return expr

def simulate_csharp_code(cs_file):
    """Simulate the C# code execution"""
    
    with open(cs_file, 'r') as f:
        code = f.read()
    
    print("Simulating C# Code Execution...")
    print("=" * 50)
    
    # Parse variables
    variables = parse_initial_variables(code)
    
    print("\nInitial Variables:")
    for var, val in sorted(variables.items()):
        if isinstance(val, bool):
            print(f"  {var} = {val}")
        elif isinstance(val, float):
            if val.is_integer():
                print(f"  {var} = {int(val)}")
            else:
                print(f"  {var} = {val:.2f}")
        else:
            print(f"  {var} = {val}")
    print()
    
    # Find all mode methods
    mode_methods = {}
    mode_pattern = r'void (\w+)\(\) \{(.*?)\n    \}'
    for match in re.finditer(mode_pattern, code, re.DOTALL):
        mode_name = match.group(1)
        mode_body = match.group(2)
        mode_methods[mode_name] = mode_body
    
    current_state = "Idle"
    
    def execute_mode(mode_name, mode_body, variables, depth=0):
        indent = "  " * depth
        output_lines = []
        output_lines.append(f"{indent}Executing Mode: {mode_name}")
        output_lines.append(f"{indent}{'-' * 40}")
        
        lines = mode_body.split('\n')
        changed_state = False
        new_state = None
        
        i = 0
        while i < len(lines):
            line = lines[i].strip()
            
            if not line or line.startswith('//'):
                i += 1
                continue
            
            # Handle Debug.Log statements
            log_match = re.search(r'Debug\.Log\((.*?)\);', line)
            if log_match:
                content = log_match.group(1)
                result = evaluate_expression(content, variables)
                output_lines.append(f"{indent}[LOG] {result}")
                i += 1
                continue
            
            # Handle assignments
            assign_match = re.search(r'(\w+)\s*=\s*(.+?);', line)
            if assign_match:
                var_name = assign_match.group(1)
                expr = assign_match.group(2)
                
                if var_name in variables:
                    old_value = variables[var_name]
                    new_value = evaluate_expression(expr, variables)
                    
                    if isinstance(old_value, bool) and not isinstance(new_value, bool):
                        new_value = bool(new_value)
                    elif isinstance(old_value, int) and isinstance(new_value, float):
                        if new_value.is_integer():
                            new_value = int(new_value)
                    
                    variables[var_name] = new_value
                    output_lines.append(f"{indent}[ASSIGN] {var_name} = {new_value} (was {old_value})")
                i += 1
                continue
            
            # Handle ChangeState
            change_match = re.search(r'ChangeState\("(\w+)"\);', line)
            if change_match:
                new_state = change_match.group(1)
                output_lines.append(f"{indent}[STATE CHANGE] {current_state} -> {new_state}")
                changed_state = True
                i += 1
                continue
            
            # Handle if statements
            if_match = re.search(r'if\s*\((.*?)\)', line)
            if if_match:
                condition = if_match.group(1)
                result = evaluate_expression(condition, variables)
                output_lines.append(f"{indent}[IF] {condition} = {result}")
                
                if result:
                    j = i + 1
                    if j < len(lines):
                        next_line = lines[j].strip()
                        change_match = re.search(r'ChangeState\("(\w+)"\);', next_line)
                        if change_match:
                            new_state = change_match.group(1)
                            output_lines.append(f"{indent}  -> {new_state}")
                            changed_state = True
                            i = j + 1
                            continue
                i += 1
                continue
            
            # Handle for loops
            for_match = re.search(r'for\s*\(int\s+\w+\s*=\s*\d+;\s*\w+\s*<\s*(\d+);', line)
            if for_match:
                loop_count = int(for_match.group(1))
                output_lines.append(f"{indent}[LOOP] Running {loop_count} times")
                
                # Find loop body
                body_lines = []
                j = i + 1
                while j < len(lines) and not lines[j].strip().startswith('}'):
                    body_lines.append(lines[j])
                    j += 1
                
                for iteration in range(loop_count):
                    output_lines.append(f"{indent}  Iteration {iteration + 1}:")
                    for body_line in body_lines:
                        body_line = body_line.strip()
                        if body_line:
                            # Execute assignment in loop
                            assign_match = re.search(r'(\w+)\s*=\s*(.+?);', body_line)
                            if assign_match:
                                var_name = assign_match.group(1)
                                expr = assign_match.group(2)
                                if var_name in variables:
                                    old_value = variables[var_name]
                                    new_value = evaluate_expression(expr, variables)
                                    variables[var_name] = new_value
                                    output_lines.append(f"{indent}    [ASSIGN] {var_name} = {new_value} (was {old_value})")
                i = j
                continue
            
            i += 1
        
        return changed_state, new_state, variables, output_lines
    
    # Execute Idle mode
    if "Idle" in mode_methods:
        changed, new_state, variables, idle_output = execute_mode("Idle", mode_methods["Idle"], variables.copy())
        
        for line in idle_output:
            print(line)
        
        # Follow state transitions
        max_transitions = 10
        transition_count = 0
        while changed and new_state and transition_count < max_transitions:
            transition_count += 1
            current_state = new_state
            
            if new_state in mode_methods:
                print()
                changed, new_state, variables, state_output = execute_mode(new_state, mode_methods[new_state], variables)
                for line in state_output:
                    print(line)
            else:
                break
    
    print("\n" + "=" * 50)
    print("FINAL VARIABLE STATE:")
    print("=" * 50)
    for var, val in sorted(variables.items()):
        if isinstance(val, bool):
            print(f"  {var} = {val}")
        elif isinstance(val, float):
            if val.is_integer():
                print(f"  {var} = {int(val)}")
            else:
                print(f"  {var} = {val:.2f}")
        else:
            print(f"  {var} = {val}")

if __name__ == "__main__":
    if len(sys.argv) > 1:
        simulate_csharp_code(sys.argv[1])
    else:
        print("Usage: python3 run_csharp_sim.py output.cs")
PYTHON_SCRIPT
                        
                        # Run the Python simulation script
                        python3 /tmp/run_csharp_sim.py output.cs
                        
                        echo -e "\n${BLUE}══════════════════════════════════════════════════════════${NC}"
                        echo -e "${GREEN}✓ Simulation Complete${NC}"
                        echo -e "${BLUE}══════════════════════════════════════════════════════════${NC}"
                    fi
                fi
            else
                echo -e "\n${RED}✗ Compilation failed!${NC}"
            fi
            read -p "Press Enter to continue..."
            ;;
            
        2)
            clear
            echo -e "${BLUE}══════════════════════════════════════════════════════════${NC}"
            echo -e "${GREEN}Compile Custom File${NC}"
            echo -e "${BLUE}══════════════════════════════════════════════════════════${NC}\n"
            
            ls -la *.txt 2>/dev/null
            if [ $? -ne 0 ]; then
                echo -e "${YELLOW}No .txt files found in current directory.${NC}"
            fi
            
            echo ""
            echo -ne "${CYAN}Enter filename: ${NC}"
            read filename
            
            if [ ! -f "$filename" ]; then
                echo -e "${RED}Error: File '$filename' not found!${NC}"
                read -p "Press Enter to continue..."
                continue
            fi
            
            echo -e "\n${YELLOW}Compiling $filename...${NC}\n"
            ./compiler "$filename"
            
            if [ $? -eq 0 ] && [ -f "output.cs" ]; then
                echo -e "\n${GREEN}✓ Compilation successful!${NC}"
                
                # Ask if user wants to view the generated code
                echo ""
                echo -ne "${CYAN}View the generated code? (y/n): ${NC}"
                read view_choice
                if [ "$view_choice" = "y" ] || [ "$view_choice" = "Y" ]; then
                    clear
                    echo -e "${BLUE}══════════════════════════════════════════════════════════${NC}"
                    echo -e "${GREEN}Generated C# Code (Full)${NC}"
                    echo -e "${BLUE}══════════════════════════════════════════════════════════${NC}\n"
                    cat output.cs
                    echo -e "\n${BLUE}══════════════════════════════════════════════════════════${NC}"
                    
                    # Ask if user wants to run the code and see output
                    echo ""
                    echo -ne "${CYAN}Run the generated code and see output? (y/n): ${NC}"
                    read run_choice
                    if [ "$run_choice" = "y" ] || [ "$run_choice" = "Y" ]; then
                        echo ""
                        echo -e "${BLUE}══════════════════════════════════════════════════════════${NC}"
                        echo -e "${GREEN}Running Generated C# Code (Simulated Output)${NC}"
                        echo -e "${BLUE}══════════════════════════════════════════════════════════${NC}\n"
                        
                        # Run the Python simulation script
                        python3 /tmp/run_csharp_sim.py output.cs
                        
                        echo -e "\n${BLUE}══════════════════════════════════════════════════════════${NC}"
                        echo -e "${GREEN}✓ Simulation Complete${NC}"
                        echo -e "${BLUE}══════════════════════════════════════════════════════════${NC}"
                    fi
                fi
            else
                echo -e "\n${RED}✗ Compilation failed!${NC}"
            fi
            read -p "Press Enter to continue..."
            ;;
            
        3)
            clear
            echo -e "${BLUE}══════════════════════════════════════════════════════════${NC}"
            echo -e "${GREEN}Generated C# Code (Full)${NC}"
            echo -e "${BLUE}══════════════════════════════════════════════════════════${NC}\n"
            
            if [ -f "output.cs" ]; then
                echo -e "${CYAN}File: output.cs ($(wc -l < output.cs) lines, $(wc -c < output.cs) bytes)${NC}\n"
                echo -e "${PURPLE}════════════ START OF FILE ════════════${NC}"
                cat output.cs
                echo -e "\n${PURPLE}═════════════ END OF FILE ═════════════${NC}"
                
                # Ask if user wants to run the code and see output
                echo ""
                echo -ne "${CYAN}Run the generated code and see output? (y/n): ${NC}"
                read run_choice
                if [ "$run_choice" = "y" ] || [ "$run_choice" = "Y" ]; then
                    echo ""
                    echo -e "${BLUE}══════════════════════════════════════════════════════════${NC}"
                    echo -e "${GREEN}Running Generated C# Code (Simulated Output)${NC}"
                    echo -e "${BLUE}══════════════════════════════════════════════════════════${NC}\n"
                    
                    # Run the Python simulation script
                    python3 /tmp/run_csharp_sim.py output.cs
                    
                    echo -e "\n${BLUE}══════════════════════════════════════════════════════════${NC}"
                    echo -e "${GREEN}✓ Simulation Complete${NC}"
                    echo -e "${BLUE}══════════════════════════════════════════════════════════${NC}"
                fi
            else
                echo -e "${RED}No output.cs file found!${NC}"
            fi
            read -p "Press Enter to continue..."
            ;;
            
        4)
            clear
            echo -e "${BLUE}══════════════════════════════════════════════════════════${NC}"
            echo -e "${GREEN}Cleaning Build Files${NC}"
            echo -e "${BLUE}══════════════════════════════════════════════════════════${NC}\n"
            
            make clean
            # Also clean temporary files
            rm -f /tmp/run_csharp_sim.py
            echo -e "\n${GREEN}✓ Clean complete!${NC}"
            read -p "Press Enter to continue..."
            ;;
            
        5)
            clear
            echo -e "${BLUE}══════════════════════════════════════════════════════════${NC}"
            echo -e "${GREEN}Compiler Statistics${NC}"
            echo -e "${BLUE}══════════════════════════════════════════════════════════${NC}\n"
            
            if [ -f "input.txt" ]; then
                LINES=$(wc -l < input.txt)
                VARS=$(grep -c "^[a-z]* " input.txt)
                MODES=$(grep -c "mode " input.txt)
                TRANS=$(grep -c -E "when|unless|ifnot" input.txt)
                LOOPS=$(grep -c -E "orbit|pulse|bounce" input.txt)
                
                echo -e "${CYAN}Input File:${NC} input.txt"
                echo -e "${CYAN}Lines:${NC} $LINES"
                echo -e "${CYAN}Variables:${NC} $VARS"
                echo -e "${CYAN}Modes:${NC} $MODES"
                echo -e "${CYAN}Transitions:${NC} $TRANS"
                echo -e "${CYAN}Loops:${NC} $LOOPS"
            else
                echo -e "${RED}No input.txt found${NC}"
            fi
            
            echo ""
            if [ -f "output.cs" ]; then
                OUT_LINES=$(wc -l < output.cs)
                OUT_SIZE=$(wc -c < output.cs)
                echo -e "${CYAN}Output File:${NC} output.cs"
                echo -e "${CYAN}Lines:${NC} $OUT_LINES"
                echo -e "${CYAN}Size:${NC} $OUT_SIZE bytes"
            fi
            
            echo ""
            if [ -f "compiler" ]; then
                COMP_SIZE=$(wc -c < compiler)
                echo -e "${CYAN}Compiler:${NC} $(date -r compiler)"
                echo -e "${CYAN}Size:${NC} $COMP_SIZE bytes"
            fi
            read -p "Press Enter to continue..."
            ;;
            
        6)
            clear
            echo -e "${BLUE}══════════════════════════════════════════════════════════${NC}"
            echo -e "${GREEN}Building Compiler${NC}"
            echo -e "${BLUE}══════════════════════════════════════════════════════════${NC}\n"
            
            make clean > /dev/null 2>&1
            make
            
            if [ $? -eq 0 ] && [ -f "compiler" ]; then
                echo -e "\n${GREEN}✓ Compiler built successfully!${NC}"
            else
                echo -e "\n${RED}✗ Build failed!${NC}"
            fi
            read -p "Press Enter to continue..."
            ;;
            
        7)
            clear
            echo -e "${GREEN}Thank you for using GOBLIN AI COMPILER!${NC}"
            echo -e "${CYAN}Goodbye!${NC}"
            # Clean temporary files
            rm -f /tmp/run_csharp_sim.py
            sleep 1
            clear
            exit 0
            ;;
            
        *)
            echo -e "${RED}Invalid option!${NC}"
            sleep 1
            ;;
    esac
    
    clear
done
import os
import subprocess
import shutil
import glob

# Archivos c++
programa = ["main.cpp", "scanner.cpp", "token.cpp", "parser.cpp", "ast.cpp", "visitor.cpp"]

# Compilar
compile = ["g++"] + programa
print("Compilando:", " ".join(compile))
result = subprocess.run(compile, capture_output=True, text=True)

if result.returncode != 0:
    print("Error en compilación:\n", result.stderr)
    exit(1)

print("Compilación exitosa")
executable = "a.exe" if os.name == "nt" else "a.out"

# Ejecutar
input_dir = "inputs"
output_dir = "outputs"
os.makedirs(output_dir, exist_ok=True)

for filepath in sorted(glob.glob(os.path.join(input_dir, "input*.txt"))):
    filename = os.path.basename(filepath)
    stem = os.path.splitext(filename)[0]
    input_number = stem.replace("input", "")
    filepath = os.path.join(input_dir, filename)

    if os.path.isfile(filepath):
        print(f"Ejecutando {filename}")
        run_cmd = [os.path.join(".", executable), filepath]
        result = subprocess.run(run_cmd, capture_output=True, text=True)

        
        # Archivos generados
        tokens_file = os.path.join(input_dir, f"{stem}.s")  # se crea en inputs/
      

        # Mover archivo de tokens si existe
        if os.path.isfile(tokens_file):
            dest_tokens = os.path.join(output_dir, f"input_{input_number}.s")
            shutil.move(tokens_file, dest_tokens)


    else:
        print(filename, "no encontrado en", input_dir)

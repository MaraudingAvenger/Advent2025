import os
import sys

def main():
    base_dir = os.path.dirname(os.path.abspath(__file__))
    for folder in sorted(os.listdir(base_dir)):
        if os.path.isdir(os.path.join(base_dir, folder)) and folder.startswith('day'):
            day_path = os.path.join(base_dir, folder)

            fn = os.path.join(day_path, 'input.txt')
            pyfile = os.path.join(day_path, 'main.py')
            
            if not os.path.exists(fn) or not os.path.exists(pyfile):
                print(f"Skipping {folder}: missing input.txt or main.py")
                continue
            
            print(f"\nRunning {folder}...")
            
            os.chdir(day_path)
            
            pycode = compile(open(pyfile).read(), pyfile, 'exec')
            exec(pycode, {'__name__': '__main__'})
            
            os.chdir(base_dir)

if __name__ == '__main__':
    main()
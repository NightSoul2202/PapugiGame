import os

source_dir = r"./Source" 
old_prefix = "Coop"
new_prefix = "Coop_"


exclude_list = [
    "FSaveMove_Coop",
    "FNetworkPredictionData_Client_Coop",
    "NewCoopMove"
]

redirects = []

for root, dirs, files in os.walk(source_dir):
    for filename in files:
        if not filename.endswith(('.h', '.cpp', '.cs')):
            continue

        filepath = os.path.join(root, filename)

        with open(filepath, 'r', encoding='utf-8') as file:
            content = file.read()

        for i, exc in enumerate(exclude_list):
            content = content.replace(exc, f"__IGNORE_TAG_{i}__")

        if old_prefix in content or old_prefix.upper() in content:
            content = content.replace(old_prefix, new_prefix)
            content = content.replace(old_prefix.upper(), new_prefix.upper())

        for i, exc in enumerate(exclude_list):
            content = content.replace(f"__IGNORE_TAG_{i}__", exc)

        with open(filepath, 'w', encoding='utf-8') as file:
            file.write(content)

        name_without_ext = os.path.splitext(filename)[0]

        if old_prefix in filename and name_without_ext not in exclude_list:
            new_filename = filename.replace(old_prefix, new_prefix)
            new_filepath = os.path.join(root, new_filename)
            os.rename(filepath, new_filepath)

            if filename.endswith('.h'):
                old_class_name = name_without_ext
                new_class_name = new_filename.replace('.h', '')
                redirects.append(f'+ClassRedirects=(OldName="/Script/PapugiGame.{old_class_name}",NewName="/Script/PapugiGame.{new_class_name}")')

print("\n=== ДОДАЙ У Config/DefaultEngine.ini ===")
print("[CoreRedirects]")
for r in set(redirects):
    print(r)
print("=========================================\n")
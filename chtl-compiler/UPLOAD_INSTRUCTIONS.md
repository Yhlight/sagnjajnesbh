# Instructions for Uploading Windows Libraries

## Step 1: Build Libraries on Windows

1. Open **Visual Studio Developer Command Prompt**
2. Navigate to the project directory
3. Run the build script:
   ```batch
   build_all_windows.bat
   ```

## Step 2: Verify Build Output

Check that these files were created:
- `build/antlr/chtl_css.lib`
- `build/antlr/chtl_javascript.lib`
- `build/antlr/antlr4-runtime.lib`
- `build/antlr/chtl_antlr_all.lib`

## Step 3: Prepare for Upload

1. Copy Release libraries:
   ```batch
   xcopy /Y build\antlr\*.lib prebuilt\windows\x64\Release\
   ```

2. Build Debug version and copy:
   ```batch
   REM Rebuild in Debug mode
   REM Copy to prebuilt\windows\x64\Debug\ with _d suffix
   ```

## Step 4: Setup Git LFS (for large files)

```batch
git lfs track "*.lib"
git add .gitattributes
```

## Step 5: Commit and Push

```batch
git add prebuilt/windows/
git commit -m "Add Windows prebuilt ANTLR libraries"
git push
```

## Alternative: GitHub Release

Instead of committing large binaries:

1. Create a GitHub Release
2. Attach the .lib files as release assets
3. Update `download_prebuilt.bat` to download from releases

## File Structure

```
prebuilt/
└── windows/
    └── x64/
        ├── Release/
        │   ├── chtl_css.lib
        │   ├── chtl_javascript.lib
        │   ├── antlr4-runtime.lib
        │   └── chtl_antlr_all.lib
        └── Debug/
            ├── chtl_css_d.lib
            ├── chtl_javascript_d.lib
            ├── antlr4-runtime_d.lib
            └── chtl_antlr_all_d.lib
```

## Automated Build

Use GitHub Actions workflow (`.github/workflows/build_windows_libs.yml`) to automatically build and upload libraries on each commit.
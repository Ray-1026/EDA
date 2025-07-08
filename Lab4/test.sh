cd KF-ATPG
echo "Running ATPG tests..."
echo ""

./KF_ATPG -atpg R -cir example/c17.bench -path_not example/c17.path_not -output ../c17_R.pttn
./KF_ATPG -atpg NR -cir example/c17.bench -path_not example/c17.path_not -output ../c17_NR.pttn
./KF_ATPG -atpg R -cir example/s27.bench -path_not example/s27.path_not -output ../s27_R.pttn
./KF_ATPG -atpg NR -cir example/s27.bench -path_not example/s27.path_not -output ../s27_NR.pttn

cd ..
echo ""
echo "Comparing results..."
diff c17_R.pttn case1_ans/c17_R.pttn
diff c17_NR.pttn case1_ans/c17_NR.pttn
diff s27_R.pttn case2_ans/s27_R.pttn
diff s27_NR.pttn case2_ans/s27_NR.pttn
if [ $? -eq 0 ]; then
    echo "All tests passed successfully!"
    rm -rf c17_R.pttn c17_NR.pttn s27_R.pttn s27_NR.pttn
else
    echo "Some tests failed. Please check the output above."
fi
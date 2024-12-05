# log_files=$(find ./Dataset/ -name "*OrderID.log")
log_files=$(find ./DatasetTest/ -name "*OrderID.log")
# echo "Deleting log files: $log_files"
rm -rf $log_files
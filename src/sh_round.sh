  ROUNDTIME=$1
  ROUND=1
if [ "$ROUNDTIME" == "" ]
then
        exit 0;
fi
while [ "1" = "1" ]
do

	#$ROUND =$ROUND+1
       # echo "=============($ROUND)==============look"
        ./status_report
	 sleep "$ROUNDTIME"
	sync
done

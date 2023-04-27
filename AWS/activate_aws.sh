KEYS_FOLDER=~/.keys
EC2KEY=ec2sshkey.pem
EC2_INSTANCE=ec2-user@18.222.161.150

if [[ -d $KEYS_FOLDER ]]; then
    echo "keys folder exists"
else
    mkdir ~/.keys
    chmod 0600 ~/.keys
    echo "successfully added Key Folder"
fi

if [[ -f $KEYS_FOLDER"/"$EC2KEY ]]; then
    echo "AWS EC2 key exists"
else
    cp ../.keys/$EC2KEY $KEYS_FOLDER"/"$EC2KEY
    chmod 0400 $KEYS_FOLDER"/"$EC2KEY
    echo "successfully added EC2 Key"
fi


# ssh -i $KEYS_FOLDER"/"$EC2KEY $EC2_INSTANCE
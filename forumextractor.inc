<?
function display_unapproved_topics()
{
    // if all posts have been approved or deleted, delete all unapproved topics
    global $mysqli;
    $result = $mysqli->query("	SELECT post_id
						FROM phpbb_posts
						WHERE post_approved = 0");
    $cunt = $result->num_rows;
    if ($cunt == 0) {
        echo "this section displayed because there are $cunt unapproved posts<br>";
        $result = $mysqli->query("SELECT topic_id, topic_poster, topic_time, topic_title
						   FROM phpbb_topics
						   WHERE topic_approved = 0
						  ");
        $cunt2 = $result->num_rows;

        while ($row = $result->fetch_array()) {
            $topic_poster = $row['topic_poster'];
            $topic_title = $row['$topic_title'];
            $topic_time = $row['topic_time'];
            $topic_time_string = date("Y-m-d", $topic_time);
            $result_unapprovedtopicposters = $mysqli->query("SELECT username, user_email, user_id, user_regdate, user_ip
																  FROM phpbb_users
																  WHERE user_id = '$topic_poster'
																  order by username
																  ");
            $row2 = $result_unapprovedtopicposters->fetch_array();
            $username = $row2['username'];
            $user_email = $row2['user_email'];
            $user_regdate = $row2['user_regdate'];
            $user_regdate_string = date("Y-m-d", $user_regdate);
            $user_ip = $row2['user_ip'];

            $result_apr_posts_by_spammer = $mysqli->query("SELECT topic_id, topic_poster, topic_time
									   FROM phpbb_topics
									   WHERE topic_approved = 1
									   AND topic_poster = 'topic_poster'
									  ");
            $num_apr_posts = $result_apr_posts_by_spammer->num_rows;
            if ($num_apr_posts > 0) $apr_post_warn = " ATTENTION - spammer also has $num_apr_posts approved posts.";

            echo "Spam topic $topic_title posted $topic_time_string by spammer who joined on $user_regdate_string
					$username ($user_email $user_ip) $apr_post_warn<br>";
        }


        echo "there are $cunt2 unapproved topics<br>";
        echo "	<form method=\"POST\" action=\"forumextractor.php\" style=\"text-align: left; margin: 0\">
			<input type=\"hidden\" name=\"page\" value=\"del_unapprovedtopicsandposters\">
			<input type=\"submit\" id=\"mysubmit\" value=\"delete unapproved topics and posters and ban\" name=\"B2\" >
			</form>";
    }
}
function display_add_banned_word_table()
{
    echo "<table>
	<tr>
		<td>
			<form method=\"POST\" target=\"newbanned\" action=\"forumextractor.php\" style=\"text-align: right; margin: 0\">
			<input type=\"text\" name=\"new_banned_word\" size=30>
			<input type=\"hidden\" name=\"page\" value=\"newbannedword\">
			<input type=\"submit\" id=\"mysubmit\" value=\"add banned word\" name=\"B2\" >
			</form>
		</td>
	</tr>
	<tr>
		<td>
			<iframe height=25 width=500 name=\"newbanned\"></iframe>
		</td>
	</tr>
</table>
";
}
function add_banned_word_to_blacklist()
{
    global $mysqli;
    $new_banned_word = $_POST[new_banned_word];
    if (!is_null($new_banned_word)) {
        $mysqli->query("INSERT INTO `banned_words` (`banned_word`) VALUES ('$new_banned_word')");
        echo "$new_banned_word - added to banned_words.";
    }
    exit;
}
function display_deleted_posts_due_to_banned_word_hits()
{
    global $mysqli;
    $sql="select banned_word
		from banned_words
		order by banned_word
		";
    $result=$mysqli->query($sql);
    while($row = $result->fetch_array()){
        $banned_word=$row['banned_word'];

        $result2 = $mysqli->query("SELECT post_id FROM phpbb_posts
							WHERE post_approved = 0
							AND (post_text like '%$banned_word%' OR post_subject like '%$banned_word%')
							");
        $result3 = $mysqli->query("DELETE FROM phpbb_posts
							WHERE post_approved = 0
							AND (post_text like '%$banned_word%' OR post_subject like '%$banned_word%')
							");
        $cunt = $result2->num_rows;
        if($cunt)echo"deleted $cunt posts with banned word $banned_word<br>";
    }
}
function private_messages_button()
{
    echo "
    <form method=\"POST\" action=\"forumextractor.php\" style=\"text-align: left; margin: 0\">
    <input type=\"hidden\" name=\"page\" value=\"privatemessages\">
    <input type=\"submit\" id=\"mysubmit\" value=\"private messages\" name=\"B2\" >
    </form>";
}
function deleteunapprovedtopics()
{
    global $mysqli, $ban_end;
    $result5 = $mysqli->query("SELECT topic_poster
					   FROM phpbb_topics
					   WHERE topic_approved = 0
					   GROUP BY topic_poster
					 ");

    while ($row5 = $result5->fetch_array()) {
        $topic_poster = $row5['topic_poster'];
        $result_spammers = $mysqli->query("SELECT user_id, user_ip, username
										FROM phpbb_users
										WHERE user_id = '$topic_poster'
										");
        $row2 = $result_spammers->fetch_array();
        $user_ip = $row2['user_ip'];
        $username = $row2['username'];

        $mysqli->query("INSERT INTO `phpbb_banlist` (`ban_userid`) VALUES ('$topic_poster')");
        $mysqli->query("INSERT INTO `phpbb_banlist` (`ban_ip`, `ban_end`) VALUES ('$user_ip', '$ban_end')");
        echo "$username banned, $user_ip banned<br>";
    }

    // DELETE UNAPPROVED TOPICS
    $mysqli->query("DELETE FROM phpbb_topics WHERE topic_approved = 0");
    echo "unapproved topics deleted (deleteunapprovedtopics())";

}
function delete_spam()
{
    global $mysqli, $ban_end;
    for ($c = 1; $c <= $_POST[C]; $c++) {
        $post_id_index = "post_id$c";                //transferred variable name is id1,id2,etc. and the variable value is 1,2,etc.
        $post_id = "$_POST[$post_id_index]";

        $del_index = "del$c";
        $del = "$_POST[$del_index]"; // $del is -1 if box checked.  null or "" if not checked
        if ($del == -1) {
            $result = $mysqli->query("SELECT poster_id, poster_ip FROM phpbb_posts WHERE post_id = $post_id");
            $row = $result->fetch_array();
            $poster_id = $row['poster_id'];
            $poster_ip = $row['poster_ip'];

            $result = $mysqli->query("DELETE FROM phpbb_posts
												WHERE poster_id = $poster_id
												OR poster_ip = '$poster_ip'");
            $result = $mysqli->query("DELETE FROM phpbb_topics
												WHERE topic_poster = $poster_id
												");
            $result = $mysqli->query("INSERT INTO `phpbb_banlist` (`ban_userid`) VALUES ('$poster_id')");
            $result = $mysqli->query("INSERT INTO `phpbb_banlist` (`ban_ip`, `ban_end`)
																	VALUES ('$poster_ip', '$ban_end')");
            echo "$post_id deleted";
        }
    }
    echo "<br>updated";
    echo "ip ban til ";
    echo date('Y-m-d', $ban_end);
    exit;
}
function private_messages()
{
    global $mysqli;
    echo "<br><br><b>PM</b>";
    $sql = "select message_time, message_subject, message_text, author_ip, author_id,
		right(to_address,length(to_address)-2) as to_short,
		u1.user_ip as user_ip, u1.username as username, u1.user_email as user_email,
		u2.username as to_user
	  from phpbb_privmsgs, phpbb_users as u1, phpbb_users as u2
	  where u1.user_id = author_id
	  and u2.user_id = right(to_address,length(to_address)-2)
	  order by msg_id desc
	  limit 40
		";

    $result = $mysqli->query($sql);

    while ($row = $result->fetch_array()) {
        extract($row);
        $message_time = date("m/d h:ia", $message_time);
        //$message_subject = $row['message_subject'];
        $message_text = $row['message_text'];
        $author_ip = $row['author_ip'];
        $author_id = $row['author_id'];
        $to_address = $row['to_address'];

        $username = $row['username'];
        $user_email = $row['user_email'];
        $user_ip = $row['user_ip'];

        $to_user = $row['to_user'];

        echo "<br><strong><big>&middot;</big></strong>
	<font size=2>$message_time</font> <font size=2>$user_email</font><br>$username -> $to_user >>> <b>$message_subject</b><br> $message_text<br>";

    }
    exit;
}

#region mysqli connect
//deprecated mysql - replaced by mysqli.  this is only still here in case i forgot to update any mysql
//$connection = mysql_connect ($db_host, $db_user, $db_password) or die (mysql_error());
//mysql_select_db ($db);

$mysqli = new mysqli($db_host,$db_user,$db_password,$db);
if ($mysqli->connect_errno)echo "Failed to connect to MySQL: (" . $mysqli->connect_errno . ") " . $mysqli->connect_error;
#endregion

$ban_end = time() + (7 * 24 * 60 * 60);
define('BAN_END',$ban_end);
// alternate pages
if($_POST[page]=="newbannedword")add_banned_word_to_blacklist();
if($_POST[page]=="deletespam")delete_spam();
if($_POST[page]=="privatemessages")private_messages();
if($_POST[page]=="del_unapprovedtopicsandposters")deleteunapprovedtopics();

// main page display - all functions display directly, and exit when they need to.

$time1 = microtime(true);
display_add_banned_word_table();
display_deleted_posts_due_to_banned_word_hits();
$time2 = microtime(true);
$time = $time2 - $time1;
echo"above here took $time seconds<br>";
$time1 = microtime(true);
echo"start timing display_unapproved_posts()";
include("/home/COMMON/forumextractor/unapprovedposts.inc"); // the function is in this file
display_unapproved_posts();
$time2 = microtime(true);
$time = $time2 - $time1;
echo"unapproved posts took $time seconds";
$time1 = microtime(true);
display_unapproved_topics();
private_messages_button();
$time2 = microtime(true);
$time = $time2 - $time1;
echo"rest of page took $time seconds<br>";


?>
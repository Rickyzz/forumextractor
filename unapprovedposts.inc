<?
// this is a comment
function unapproved_posts_query(){
	global $mysqli;
	$sql = "select a.post_subject, a.post_text, a.poster_id, a.poster_ip, a.post_time, a.post_id, a.topic_id,
			 ua.user_ip, ua.username, ua.user_email,
			 ub.username as username_b, ub.user_email as user_email_b,
			 b.post_subject as post_subject_b, b.post_text as post_text_b

		from phpbb_posts a
			inner join phpbb_users ua on ua.user_id = a.poster_id
			left join phpbb_posts b on (a.poster_ip=b.poster_ip and a.poster_id != b.poster_id)
			left join phpbb_users ub on ub.user_id = b.poster_id

		where a.post_approved = 0
		order by a.post_id desc
		limit 50
		";
	return $mysqli->query($sql);
}
function clean_text($t){
	$t = str_replace("<","{",$t);
	$t = str_replace("[","{",$t);
	//$t = preg_replace("/<.*>/i", " <html> ", $t);
	//$t = preg_replace("/\[.+\]/i", " [bbcode] ", $t);
	$t = filter_var($t, FILTER_SANITIZE_SPECIAL_CHARS, FILTER_FLAG_STRIP_LOW);
	return substr($t, 0, 800);
}
function display_unapproved_posts()
{
	global $db, $url, $forum_u, $forum_p;
	$xtoday = date("m/d/y");
	echo "<br><br>Today <font size=1>$xtoday</font>"
		.auto_login_button($url, $forum_u, $forum_p)
		."<br>";

	$result = unapproved_posts_query();
	// variables ending with _b are for ip matches of previous posts

	echo "<table border=1><tr>
	<td bgcolor=grey>user/email</td>
	<td bgcolor=grey>subject/body/time</td>
	<td bgcolor=grey>delete</td>
	</tr>
<form method=\"POST\" target=\"myiframe1\" action=\"forumextractor.php\" style=\"text-align: right; margin: 0\">";

	while ($row = $result->fetch_array()) {
		$c++;
		$post_id[$c] = $row['post_id'];
		$post_time = date("m/d h:ia", $row['post_time']);
		$post_subject = $row['post_subject'];
		$post_subject = filter_var($post_subject, FILTER_SANITIZE_SPECIAL_CHARS, FILTER_FLAG_STRIP_LOW);
		$post_subject_b = $row['post_subject_b'];
		$post_subject_b = filter_var($post_subject_b, FILTER_SANITIZE_SPECIAL_CHARS, FILTER_FLAG_STRIP_LOW);
		$post_text = clean_text($row['post_text']);
		$post_text_b = clean_text($row['post_text_b']); // old post, same ip
		$username = $row['username'];
		$username_b = $row['username_b'];
		$user_email = $row['user_email'];
		$user_email_b = $row['user_email_b'];
		$topic_id = $row['topic_id'];

		if($lastpostid != $post_id[$c]) {

			// so it does not repeat unapproved post for every matching previous post
			// ok to display matching unapproved posts for each post from same user so know if many new users same ip
			// t=$topic_id
			echo "
		<tr>
			<td>$username<br>$user_email</td>
			<td><a target=_blank href=\"/forum/viewtopic.php?p=$post_id[$c]\">$post_subject</a>
			 	<font size=2>($post_time)</font><br>$post_text</td>
			<td><input type=\"hidden\" name=\"post_id$c\" value=$post_id[$c]>
				<input type=\"checkbox\" name=\"del$c\" value=\"-1\">
			</td>
		</tr>";
		}
		$lastpostid = $post_id[$c];

		// other posts from same ip
		if($post_subject_b) {
			echo "
			<tr>
			<td></td>
			<td bgcolor=grey><font color=red>same ip $username_b $user_email_b</font> $post_subject_b<br>$post_text_b</td>
			</tr>
		";
		}
/*
		// other users posts from same ip, old query
		// perhaps should copy the 4th line into the new query
		select post_subject, post_text, username, user_email
		from phpbb_posts, phpbb_users
		where user_id = poster_id
		and (poster_ip='$poster_ip' or poster_ip='$user_ip' or user_ip='$poster_ip' or user_ip='$poster_ip')
		and poster_id <> '$poster_id'
		order by post_id desc
		limit 20
*/
	}
	echo "
	<tr>
		<td><input type=\"hidden\" name=\"C\" value=$c>
			<input type=\"hidden\" name=\"page\" value=\"deletespam\">
			<input type=\"submit\" id=\"mysubmit\" value=\"delete spam\" name=\"B2\" style=\"height: 25\"></form></td>
		<td><iframe name=\"myiframe1\" height=100 width=200 marginheight=0 marginwidth=5 scrolling=no></iframe></td>
		<td></td>
	</tr>
</table>";

}
function auto_login_button($url, $u, $p, $new_tab=TRUE){
	//$forum_url="https://www.snailzoom.com/forum/ucp.php";
	//$forumu=UFAT;
	//if(UFAT=="h"){$forumu="richard";}
	if($new_tab==TRUE)$target = "target=_blank";
	return"<form action=\"$url\" $target method=\"post\"
                style=\"text-align: left; line-height: 100%; margin-top: 0; margin-bottom: 0\">
                <input type=\"hidden\" name=\"username\" value=\"$u\">
                <input type=\"hidden\" name=\"password\" value=\"$p\">
                <input type=\"hidden\" name=\"redirect\" value=\"index.php\">
                <input type=\"submit\" id=\"mysubmit\" value=\"forum\" name=\"login\" style=\"width: 40\">
                </form>";


}
?>